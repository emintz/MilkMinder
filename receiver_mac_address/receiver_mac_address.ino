#include "Arduino.h"
#include "WiFi.h"
#include "esp_wifi.h"

/**
 * Retrieve the machine's MAC address.
 */

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  uint8_t mac_address[6];
  memset(mac_address, 0, sizeof(mac_address));
  esp_err_t address_read_status = esp_wifi_get_mac(WIFI_IF_STA, mac_address);
  if (address_read_status == ESP_OK) {
    Serial.printf("MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
        mac_address[0],
        mac_address[1],
        mac_address[2],
        mac_address[3],
        mac_address[4],
        mac_address[5]);
  } else {
      Serial.println("Could not read MAC address.");
  }
}

// The loop function is called in an endless loop
void loop() {
  vTaskDelay(portMAX_DELAY);
}
