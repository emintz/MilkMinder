#include "Arduino.h"
#include "WiFi.h"
#include "esp_now.h"

/**
 * Retrieve the machine's MAC address.
 */

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());

}

// The loop function is called in an endless loop
void loop() {
}
