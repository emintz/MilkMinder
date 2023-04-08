#include "Arduino.h"

/**
 * Reads the gyroscope and sends notifications to a receiver.
 */

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_now.h"

#include "Wire.h"
#include "WiFi.h"

#include "BlinkTask.h"
#include "CommunicationSettings.h"
#include "EspNowTransmitter.h"
#include "EventRelayTask.h"
#include "GyroscopeTask.h"
#include "PinAssignments.h"

#include "MotionNotificationMessage.h"


QueueHandle_t h_gyroscope_event_queue;
QueueHandle_t h_notification_send_queue;
TaskHandle_t h_connection_dropped_blink_task;
TaskHandle_t h_gyroscope_update_task;
TaskHandle_t h_motion_detection_task;
TaskHandle_t h_event_relay_task;
TaskHandle_t h_esp_now_transmit_task;

BlinkTask connection_dropped_signal(
  "Receiver connection lost",
  RED_LED_PIN,
  3,
  150,
  500);

EspNowTransmitter esp_now_transmitter(
  receiver_address,
  &connection_dropped_signal);

GyroscopeTask gyroscope_task;

EventRelayTask event_relay_task;

void start_blink_tasks() {
  Serial.print("Starting blink task ... ");
  h_connection_dropped_blink_task =
    connection_dropped_signal.start_blink_loop("ESP_NOW connection");
  if (h_connection_dropped_blink_task) {
    Serial.print("succeeded.");
    vTaskSuspend(h_connection_dropped_blink_task);
    Serial.println("Blink task suspended.");
  } else {
    Serial.print("failed.");
  }
  EspNowTransmitter::set_blink_task(&connection_dropped_signal);
}

void setup() {
  Serial.begin(115200);
  Serial.print("Gyroscope readings sender built on ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);

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

	// Lamp test

  vTaskDelay(pdMS_TO_TICKS(1000));
  Serial.println("Illuminating LEDs.");
  digitalWrite(RED_LED_PIN, HIGH);
  vTaskDelay(pdMS_TO_TICKS(150));
  digitalWrite(YELLOW_LED_PIN, HIGH);
  vTaskDelay(pdMS_TO_TICKS(150));
  digitalWrite(GREEN_LED_PIN, HIGH);
  vTaskDelay(pdMS_TO_TICKS(150));
  digitalWrite(BLUE_LED_PIN, HIGH);
  vTaskDelay(pdMS_TO_TICKS(5000));
  Serial.println("Extinguishing LEDs.");
  digitalWrite(RED_LED_PIN, LOW);
  vTaskDelay(pdMS_TO_TICKS(150));
  digitalWrite(YELLOW_LED_PIN, LOW);
  vTaskDelay(pdMS_TO_TICKS(150));
  digitalWrite(GREEN_LED_PIN, LOW);
  vTaskDelay(pdMS_TO_TICKS(150));
  digitalWrite(BLUE_LED_PIN, LOW);

  /**
   * Initialize low-level I/O.
   */
  Serial.print("Configuring WiFi ... ");
  Serial.println(WiFi.mode(WIFI_STA) ? "succeeded." : "failed.");

  Serial.print("Initializing I2C ");
  Wire.setPins(21, 22);
  Wire.begin();
  Serial.println(" done.");

  /**
   * Create queues.
   */
  Serial.println("Creating event queues.");
  Serial.print("Gyroscope event queue ");
  h_gyroscope_event_queue =
    xQueueCreate(10, sizeof(MotionNotificationMessage));
  Serial.println(h_gyroscope_event_queue ? "created." : "failed.");

  Serial.print("Receiver notification queue ... ");
  h_notification_send_queue =
    xQueueCreate(10, sizeof(MotionNotificationMessage));
  Serial.println(h_notification_send_queue ? "created." : "failed.");
  Serial.println("Queue setup completed.");

  /**
   * Configure tasks.
   */
  Serial.println("Configuring tasks.");
  Serial.println("Gyroscope manager.");
  gyroscope_task.begin(h_gyroscope_event_queue);

  Serial.println("Event relay task.");
  event_relay_task.begin(
    h_gyroscope_event_queue,
    h_notification_send_queue);

  /**
   * Start tasks.
   */
  Serial.println("Starting tasks.");
  start_blink_tasks();

  Serial.println("Configuring ESP-NOW transmitter.");
  esp_now_transmitter.begin(h_notification_send_queue);

  h_event_relay_task = event_relay_task.start_send_loop();

  h_esp_now_transmit_task = esp_now_transmitter.start();

  h_gyroscope_update_task = gyroscope_task.start_update_loop();

  h_motion_detection_task = gyroscope_task.start_motion_detection_loop();

  Serial.println("Setup completed.");
  Serial.flush();
}

// The loop function is called in an endless loop until we cancel the
// invoking task.
void loop() {
  vTaskDelete(NULL);
}
