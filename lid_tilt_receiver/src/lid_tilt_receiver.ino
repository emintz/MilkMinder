/**
 * Milk minder receiver.
 *
 *      Author: Eric Mintz
 *
 * MilkMinder receiver
 */

#include <src/DeliveryLedAction.h>

#include "Arduino.h"

#include "driver/gpio.h"
#include "driver/timer.h"

#include <stdlib.h>

#include "WiFi.h"
#include "esp_now.h"
#include "Wire.h"
#include "RTClib.h"
#include "LiquidCrystal_I2C.h"
#include "PullQueueHT.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "soc/rtc.h"

#include <time.h>
#include <sys/time.h>

#include "BlinkAction.h"
#include "TaskWithActionH.h"

#include "AlarmAction.h"
#include "AlarmMessage.h"
#include "CommunicationEvent.h"
#include "DeliveryLEDIlluminationStatus.h"
#include "DisplayMessage.h"
#include "EspNowStatusAction.h"
#include "GyroConnectionWatchdogAction.h"
#include "LidPositionReport.h"
#include "MilkArrivalAction.h"
#include "PinAssignments.h"
#include "Priorities.h"
#include "ReceiveAction.h"
#include "RippleTask.h"
#include "StatusDisplayAction.h"
#include "TimeAction.h"
#include "Timezone.h"
#include "WhiteLedPin.h"

#define I2C_LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLUMNS 16

// TODO: store the timezone in eeprom.
static TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //UTC - 4 hours
static TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //UTC - 5 hours
static Timezone usEastern(usEDT, usEST);

// Event queues
static PullQueueHT<AlarmMessage> alarm_event_queue(3);
static PullQueueHT<ConnectionStatusMessage> connection_status_queue(3);
static PullQueueHT<LedIlluminationMessage> delivery_led_illumination_queue(3);
static PullQueueHT<DisplayMessage> display_command_queue(3);
static PullQueueHT<LidPositionReport> lid_position_report_queue(3);

static AlarmAction alarm_action(ALARM_PIN, YELLOW_LED_PIN, alarm_event_queue);
static TaskWithActionH alarm_task(
    "Alarm",
    ALARM_PRIORITY,
    &alarm_action,
    4096);

static RTC_DS3231 time_keeper;
static TimeAction time_action(
    &time_keeper, GPIO_NUM_17, &usEastern, display_command_queue);
static TaskWithActionH time_task(
    "Time Keeper",
    TIMEKEEPER_PRIORITY,
    &time_action,
    4096);

static MilkArrivalAction milk_arrival_action(
    &time_action,
    alarm_event_queue,
    delivery_led_illumination_queue,
    display_command_queue,
    lid_position_report_queue);
static TaskWithActionH milk_arrival_task(
    "Arrival",
    ARRIVAL_PRIORITY,
    &milk_arrival_action,
    4096);

static LiquidCrystal_I2C display(I2C_LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
static StatusDisplayAction status_display_action(
    display, &time_action, display_command_queue);
static TaskWithActionH status_display_task(
    "Status Display",
    STATUS_DISPLAY_PRIORITY,
    &status_display_action,
    4096);

static const uint8_t led_pins[] =
	{RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN};
#define NUMBER_OF_LED_PINS 4

static RippleTask ripple_task(led_pins, NUMBER_OF_LED_PINS, 100);

static GyroConnectionWatchdogAction gyro_connection_watchdog(
    connection_status_queue);
static TaskWithActionH gyro_connection_watchdog_task(
    "ESP Now Watchdog",
    GYRO_WATCHDOG_PRIORITY,
    &gyro_connection_watchdog,
    4096);

static ReceiveAction receive_action(
    &gyro_connection_watchdog,
    lid_position_report_queue);
static TaskWithActionH receive_task(
    "Receive",
    RECEIVE_PRIORITY,
    &receive_action,
    4096);

static DeliveryLedAction delivery_led_action(
    delivery_led_illumination_queue, BLUE_LED_PIN, 100, 100);
static TaskWithActionH delivery_led_task(
    "Delivery LED",
    DELIVERY_LED_PRIORITY,
    &delivery_led_action,
    4096);

static BlinkAction blink_red_action(RED_LED_PIN, 1, 100, 100, 1);
static TaskWithActionH blink_red_task(
    "Red Blink",
    RED_BLINK_PRIORITY,
    &blink_red_action,
    2048);

static EspNowStatusAction esp_now_status_action(
    blink_red_action,
    GREEN_LED_PIN,
    connection_status_queue,
    display_command_queue);
static TaskWithActionH esp_now_status_task(
    "Network Status",
    NETWORK_STATUS_PRIORITY,
    &esp_now_status_action,
    4096);

/**
 * Receives notification of lid tilt, which indicates that milk has been
 * delivered.
 */
void setup() {
  gpio_install_isr_service(0);
  digitalWrite(WHITE_LED_PIN, HIGH);
  ripple_task.start();
  ripple_task.resume();
  Serial.begin(115200);
  Serial.print("Milk minder receiver compiled on ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);
  Serial.print("Clock frequency is: ");
  Serial.println(rtc_clk_apb_freq_get());
  Serial.println("Test receiver is booting.");

  pinMode(BUILTIN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(WHITE_LED_PIN, OUTPUT);
  pinMode(ALARM_PIN, OUTPUT);

  digitalWrite(BUILTIN_LED_PIN, LOW);

  alarm_event_queue.begin();
  connection_status_queue.begin();
  delivery_led_illumination_queue.begin();
  display_command_queue.begin();
  lid_position_report_queue.begin();

  status_display_task.start();
  DisplayMessage display_message;
  memset(&display_message, 0, sizeof(display_message));
  display_message.command = LCD_INIT;
  display_command_queue.send_message(&display_message, 0);
  memset(&display_message, 0, sizeof(display_message));

  display_message.command = LCD_DISCONNECTED;
  display_command_queue.send_message(&display_message);

  Wire.begin();

  if (!WiFi.mode(WIFI_STA)) {
    Serial.println("Could not configure WIFI.");
    // TODO: display a error and halt.
  }
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  if (!esp_now_init() == ESP_OK) {
     Serial.println("ESP_NOW initialization failed.");
  } else {
    Serial.println("ESP_NOW initialized and ready to start.");
  }

  for (int beep_count = 0; beep_count < 5; ++beep_count) {
    digitalWrite(ALARM_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    digitalWrite(ALARM_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(20));
  }

  vTaskDelay(pdMS_TO_TICKS(10000));
  ripple_task.suspend();
  digitalWrite(WHITE_LED_PIN, LOW);

  delivery_led_task.start();

  blink_red_task.start();
  blink_red_action.blink_off();
  esp_now_status_task.start();

  alarm_task.start();

  gyro_connection_watchdog_task.start();
  Serial.println("Watchdog timer started.");
  time_action.begin();
  time_task.start();

  timeval tv;
  tv.tv_sec = time_keeper.now().unixtime();
  tv.tv_usec = 0;
  Serial.println("Setting time of day.");
  settimeofday(&tv, NULL);
  Serial.println("Time set.");

  ReceiveAction::begin();

  milk_arrival_task.start();

  receive_task.start();
  Serial.println("Receiver task started.");
  memset(&display_message, 0, sizeof(display_message));
  display_message.command = LCD_RUN;
  display_command_queue.send_message(&display_message, 0);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(10000));
}
