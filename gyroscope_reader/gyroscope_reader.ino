#include <EspNowTransmitAction.h>
#include <EventRelayAction.h>
#include <GyroUpdateAction.h>
#include <MotionDetectAction.h>

#include "Arduino.h"

/**
 * Reads the gyroscope and sends notifications to a receiver.
 */

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_now.h>
#include <esp_wifi.h>

#include "MPU6050_light.h"
#include "RippleAction.h"
#include "PullQueueHT.h"
#include "TaskWithActionH.h"
#include "Wire.h"
#include "WiFi.h"

#include "CommunicationSettings.h"
#include "PinAssignments.h"
#include "TaskPriorities.h"
#include "TaskWithActionH.h"

#include "MotionNotificationMessage.h"

static const uint8_t led_pins[] =
        {RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN};
#define NUMBER_OF_LED_PINS 4

static PullQueueHT<MotionNotificationMessage> gyroscope_event_queue(10);
static PullQueueHT<MotionNotificationMessage> notification_send_queue(10);

static TaskHandle_t h_connection_dropped_blink_task;

static MPU6050 gyroscope(Wire);

static EspNowTransmitAction esp_now_transmit_action(
    receiver_address,
    notification_send_queue);
static TaskWithActionH esp_now_transmit_task(
    "ESP-Now transmit",
    ESP_NOW_SEND_PRIORITY,
    &esp_now_transmit_action,
    4096);

static GyroUpdateAction gyro_update_action(gyroscope);
static TaskWithActionH gyro_refresh_task(
    "Refresh Gyro",
    GYROSCOPE_UPDATE_PIORITY,
    &gyro_update_action,
    4096);

static MotionDetectAction motion_detect_action(
    gyroscope_event_queue,
    gyroscope);
static TaskWithActionH motion_detect_task(
    "Detect Motion",
    MOTION_DETECTION_PRIORITY,
    &motion_detect_action,
    4096);

static EventRelayAction event_relay_action(
    gyroscope_event_queue,
    notification_send_queue);
static TaskWithActionH event_relay_task(
    "Event Relay",
    RELAY_PRIORITY,
    &event_relay_action,
    4096);

static void start_blink_tasks() {
  Serial.print("Starting blink task ... ");
  EspNowTransmitAction::begin();
}

/**
 * Configures GPIO pins that drive LEDs and runs the initial
 * lamp test.
 */
static void init_leds(void) {
  pinMode(SYSTEM_IS_LIVE_LED_PIN, OUTPUT);
  digitalWrite(SYSTEM_IS_LIVE_LED_PIN, LOW);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  Serial.print("Gyroscope readings sender built on ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);

  Serial.printf("Receiver MAC address: : %02x:%02x:%02x:%02x:%02x:%02x\n",
      receiver_address[0], receiver_address[1], receiver_address[2],
      receiver_address[3], receiver_address[4], receiver_address[5]);

  init_leds();
  RippleAction ripple_action(led_pins, NUMBER_OF_LED_PINS, 100);
  TaskWithActionH ripple_task("Ripple", RIPPLE_PRIORITY, &ripple_action, 4096);
  ripple_task.start();

  /**
   * Initialize low-level I/O.
   */
  Serial.print("Configuring WiFi ... ");
  Serial.flush();
  if (WiFi.mode(WIFI_STA)) {
    Serial.println(" ... succeeded");
    uint8_t mac_address[6];
    memset(mac_address, 0, sizeof(mac_address));
    if (ESP_OK != esp_wifi_get_mac(WIFI_IF_STA, mac_address)) {
      Serial.println("MAC address read failed.");
    } else {
      Serial.printf(
          "MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
          mac_address[0], mac_address[1], mac_address[2],
          mac_address[3], mac_address[4], mac_address[5]);
    }
  } else {
    Serial.println(" ... failed.");
  }

  Serial.print("Initializing I2C ... ");
  Serial.flush();
  Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.begin();
  Serial.println(" done.");

  /**
   * Create queues.
   */
  Serial.println("Creating event queues.");
  Serial.print("Gyroscope event queue ");
  Serial.println(gyroscope_event_queue.begin() ? "created." : "failed.");

  Serial.print("Receiver notification queue ... ");
  Serial.println(notification_send_queue.begin() ? "created." : "failed.");
  Serial.println("Queue setup completed.");

  /**
   * Initialize the gyroscope connection so that we
   * can read the lid tilt angle. The actual read
   * is started below.
   */
  gyroscope.begin();
  Serial.println("Gyroscope initialized.");

  /**
   * Configure tasks.
   */
  Serial.println("Configuring tasks.");
  Serial.println("Gyroscope manager.");
  motion_detect_action.begin();

  /**
   * Start tasks.
   */
  Serial.println("Starting tasks.");
  start_blink_tasks();

  Serial.println("Configuring ESP-NOW transmitter.");
  esp_now_transmit_action.espnow_start();

  Serial.println("Event relay task.");

  if (event_relay_task.start()) {
    Serial.println("Relay task started.");
  } else {
    Serial.println("Relay task failed to start.");
  }

  esp_now_transmit_task.start();
  Serial.println("ESP Now transmitter started");

  gyro_refresh_task.start();
  motion_detect_task.start();

  Serial.println("Setup completed.");
  Serial.flush();
  ripple_action.ripple_off();
  ripple_task.stop();
}

/**
 * The loop function is called in an endless loop. There's
 * nothing  to do so we cancel the invoking task.
 */
void loop() {
  vTaskDelete(NULL);
}
