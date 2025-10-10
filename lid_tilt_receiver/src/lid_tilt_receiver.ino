/**
 * Milk minder receiver.
 *
 *      Author: Eric Mintz
 *
 */
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
#include "freertos/queue.h"
#include "freertos/task.h"

#include "soc/rtc.h"

#include <time.h>
#include <sys/time.h>

#include "AlarmTask.h"
#include "CommunicationEvent.h"
#include "ConnectionStatusTask.h"
#include "DeliveryLEDIlluminationStatus.h"
#include "DeliveryLedTask.h"
#include "DisconnectedLedTask.h"
#include "DisplayMessage.h"
#include "GyroConnectionWatchdogTask.h"
#include "LidPositionReport.h"
#include "LCDDisplayTask.h"
#include "MilkArrivalTask.h"
#include "PinAssignments.h"
#include "ReceiverTask.h"
#include "RippleTask.h"
#include "TimeTask.h"
#include "Timezone.h"
#include "WhiteLedPin.h"

#define I2C_LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLUMNS 16

static QueueHandle_t h_alarm_event_queue;
static QueueHandle_t h_communications_event_queue;
static QueueHandle_t h_delivery_led_illumination_queue;
static QueueHandle_t h_display_command_queue;
static QueueHandle_t h_lid_position_report_queue;

static TaskHandle_t h_connection_status_task;
static TaskHandle_t h_disconnected_led_task;
static TaskHandle_t h_lcd_display_task;
static TaskHandle_t h_delivery_led_illumination_task;
static TaskHandle_t h_milk_arrival_task;
static TaskHandle_t h_time_task;

// TODO: store the timezone in eeprom.
static TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //UTC - 4 hours
static TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //UTC - 5 hours
static Timezone usEastern(usEDT, usEST);

// Event queues
static PullQueueHT<AlarmTask::AlarmTaskMessage> alarm_event_queue(3);
static PullQueueHT<CommunicationEvent> communications_event_queue(3);
static PullQueueHT<LedIlluminationMessage> delivery_led_illumination_queue(3);
static PullQueueHT<DisplayMessage> display_command_queue(3);
static PullQueueHT<LidPositionReport> lid_position_report_queue(3);

static AlarmTask alarm_task(ALARM_PIN, YELLOW_LED_PIN);

static RTC_DS3231 time_keeper;
static TimeTask time_task(&time_keeper, &usEastern);

static MilkArrivalTask milk_arrival_task(&time_task);

static LiquidCrystal_I2C display(I2C_LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
static LCDDisplayTask display_task(display, &time_task);

static const uint8_t led_pins[] =
	{RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN};
#define NUMBER_OF_LED_PINS 4

static RippleTask ripple_task(led_pins, NUMBER_OF_LED_PINS, 100);

static GyroConnectionWatchdogTask gyro_connection_watchdog;

static ReceiverTask receiver_task(&time_task, &gyro_connection_watchdog);

static DeliveryLedTask delivery_led_task(BLUE_LED_PIN, 100, 100);

static DisconnectedLedTask disconnected_led_task(RED_LED_PIN);
static ConnectionStatusTask connection_status_task(
    &disconnected_led_task, GREEN_LED_PIN);

/**
 * Receives notification of lid tilt, which indicates that milk has been
 * delivered.
 */
void setup() {
  gpio_install_isr_service(0);

  pinMode(BUILTIN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(WHITE_LED_PIN, OUTPUT);
  pinMode(ALARM_PIN, OUTPUT);

  digitalWrite(BUILTIN_LED_PIN, LOW);

  alarm_event_queue.begin();
  h_alarm_event_queue = alarm_event_queue.handle();
      // was: xQueueCreate(3, sizeof(AlarmTask::AlarmTaskMessage));
  communications_event_queue.begin();
  h_communications_event_queue = communications_event_queue.handle();
      // was: xQueueCreate(3, sizeof(CommunicationEvent));
  delivery_led_illumination_queue.begin();
  h_delivery_led_illumination_queue = delivery_led_illumination_queue.handle();
      // WasL xQueueCreate(3, sizeof(LedIlluminationMessage));
  display_command_queue.begin();
  h_display_command_queue = display_command_queue.handle();
      // Was: xQueueCreate(3, sizeof(DisplayMessage));
  lid_position_report_queue.begin();
  h_lid_position_report_queue = lid_position_report_queue.handle();
      // Was: xQueueCreate(3, sizeof(LidPositionReport));

  h_lcd_display_task = display_task.start(h_display_command_queue);
  DisplayMessage display_message;
  memset(&display_message, 0, sizeof(display_message));
  display_message.command = LCD_INIT;
  xQueueSendToBack(h_display_command_queue, &display_message, 0);
  memset(&display_message, 0, sizeof(display_message));

  display_message.command = LCD_DISCONNECTED;
  xQueueSendToBack(h_display_command_queue, &display_message, 0);

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

  h_delivery_led_illumination_task =
      delivery_led_task.start(h_delivery_led_illumination_queue);

  h_disconnected_led_task = disconnected_led_task.start(h_alarm_event_queue);

  h_connection_status_task = connection_status_task.start(
      h_communications_event_queue,
      h_display_command_queue);

  alarm_task.start(h_alarm_event_queue);

  // watchdog_timer.start(h_communications_event_queue);
  gyro_connection_watchdog.start(h_communications_event_queue);
  Serial.println("Watchdog timer started.");
  h_time_task = time_task.start(h_display_command_queue, GPIO_NUM_17);

  timeval tv;
  tv.tv_sec = time_keeper.now().unixtime();
  tv.tv_usec = 0;
  Serial.println("Setting time of day.");
  settimeofday(&tv, NULL);
  Serial.println("Time set.");

  ReceiverTask::begin();

  h_milk_arrival_task = milk_arrival_task.start(
      h_lid_position_report_queue,
      h_delivery_led_illumination_queue,
      h_alarm_event_queue,
      h_display_command_queue);

  receiver_task.start(
      h_communications_event_queue,
      h_lid_position_report_queue);
  Serial.println("Receiver task started.");
  memset(&display_message, 0, sizeof(display_message));
  display_message.command = LCD_RUN;
  xQueueSendToBack(h_display_command_queue, &display_message, 0);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(10000));
}
