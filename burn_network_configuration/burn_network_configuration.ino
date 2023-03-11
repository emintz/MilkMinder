/**
 * Burn network credentials into non volatile memory
 */

#include "Arduino.h"
#include <stdlib.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

char wifi_ssid[34];
char wifi_password[34];

const char * SSID_VALUE = "MushroomPlanet";
const char * PASSWORD_VALUE = "Basidium-X100";
const char * MILK_MINDER_NVS_NAMESPACE = "milk_minder";
const char * SSID_KEY = "ssid";
const char * PASSWORD_KEY = "password";

nvs_handle_t h_non_volitile_storage;

void setup() {
	Serial.begin(115200);
	Serial.print("Compiled on ");
	Serial.print(__DATE__);
	Serial.print(" at ");
	Serial.println(__TIME__);
	Serial.println("Initializing.");

	esp_err_t esp_status = ESP_OK;
	size_t entries_used = 0;

	esp_status = nvs_flash_init();
	if (esp_status != ESP_OK) {
		Serial.print("Flash API initialization failed with code: 0X");
		Serial.println(esp_status, 16);
	}
	Serial.println("Non-volitile API initialized.");

	esp_status = nvs_open(MILK_MINDER_NVS_NAMESPACE, NVS_READWRITE, &h_non_volitile_storage);
	if (esp_status != ESP_OK) {
		Serial.print("Opening non-volitile namespace ");
		Serial.print(MILK_MINDER_NVS_NAMESPACE);
		Serial.print(" for read/write failed with code: 0X");
		Serial.println(esp_status, 16);
	}
	Serial.println("Non-volitile storage open for read/write.");

	esp_status = nvs_set_str(h_non_volitile_storage, SSID_KEY, SSID_VALUE);
	if (esp_status != ESP_OK) {
		Serial.print("SSID value set failed with code: 0X");
		Serial.println(esp_status, 16);
	}
	Serial.println("SSID written to non-volitile storage.");

	esp_status = nvs_set_str(h_non_volitile_storage, PASSWORD_KEY, PASSWORD_VALUE);
	if (esp_status != ESP_OK) {
		Serial.print("SSID value set failed with code: 0X");
		Serial.println(esp_status, 16);
	}
	Serial.println("PASSWORD written to non-volitile storage.");

	esp_status = nvs_commit(h_non_volitile_storage);
	if (esp_status != ESP_OK) {
		Serial.println("Non-volitile storage commit failed with status: 0X");
		Serial.print(esp_status, 16);
	}
	vTaskDelay(pdMS_TO_TICKS(1000));
	Serial.println("Changes to non-volitile storage committed.");

	esp_status = nvs_get_used_entry_count(h_non_volitile_storage, &entries_used);
	if (esp_status != ESP_OK) {
		Serial.print("Retrieving entry count failed with status: 0X");
		Serial.println(esp_status, 16);
	}
	Serial.print("Number of entries is ");
	Serial.println(entries_used);

	size_t length = sizeof(wifi_ssid);
	esp_status = nvs_get_str(h_non_volitile_storage, SSID_KEY, wifi_ssid, &length);
	if (esp_status != ESP_OK) {
		Serial.print("Opening non-volitile namespace ");
		Serial.print(MILK_MINDER_NVS_NAMESPACE);
		Serial.print(" for read failed with status: 0X");
		Serial.print(esp_status, 16);
	}
	Serial.print("SSID: ");
	Serial.println(wifi_ssid);

	length = sizeof(wifi_password);
	esp_status = nvs_get_str(h_non_volitile_storage, PASSWORD_KEY, wifi_password, &length);
	if (esp_status != ESP_OK) {
		Serial.print("Opening non-volitile namespace ");
		Serial.print(MILK_MINDER_NVS_NAMESPACE);
		Serial.print(" for read failed with status: 0X");
		Serial.print(esp_status, 16);
	}
	Serial.print("Password: ");
	Serial.println(wifi_password);

	nvs_close(h_non_volitile_storage);
	h_non_volitile_storage = 0;
	Serial.println("Non-volitile storage closed.");

	esp_status = nvs_open(MILK_MINDER_NVS_NAMESPACE, NVS_READONLY, &h_non_volitile_storage);
	if (esp_status != ESP_OK) {
		Serial.print("Opening non-volitile namespace ");
		Serial.print(MILK_MINDER_NVS_NAMESPACE);
		Serial.print(" for read/write failed with code: 0X");
		Serial.println(esp_status, 16);
	}
	Serial.println("Non-volitile storage open for read/write.");

	length = sizeof(wifi_ssid);
	esp_status = nvs_get_str(h_non_volitile_storage, SSID_KEY, wifi_ssid, &length);
	if (esp_status != ESP_OK) {
		  Serial.print("Retrieving network SSID failed with status: 0X");
		  Serial.println(esp_status, 16);
	}
	Serial.print("SSID: ");
	Serial.println(wifi_ssid);

	length = sizeof(wifi_password);
	esp_status = nvs_get_str(h_non_volitile_storage, PASSWORD_KEY, wifi_password, &length);
	if (esp_status != ESP_OK) {
			Serial.print("Retrieving network password failed with status: 0X");
			Serial.println(esp_status, 16);
	}
	Serial.print("Password: ");
	Serial.println(wifi_password);

	nvs_close(h_non_volitile_storage);
	esp_status = nvs_flash_deinit();
	if (esp_status != ESP_OK) {
		Serial.print("Shutting down the flash API failed with status 0X");
		Serial.println(esp_status, 16);
	}
	Serial.println("Flash API shut down.");
	Serial.println("Program ends.");
}

// The loop function is called in an endless loop
void loop() {
}
