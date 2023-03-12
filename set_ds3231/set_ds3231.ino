#include "Arduino.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "NTPClient.h"
#include "Wire.h"
#include "RTClib.h"

/**
 * Uses NTP to set a DS3231 to the current UTC time. Please be sure to
 * set your wireless network name and password below.
 */

String formatted_date;
RTC_DS3231 time_keeper;
WiFiUDP ntpUDP;
NTPClient time_client(ntpUDP);
int number_of_times = 0;

char formatted_digit[4];

void shutdown() {
        disableCore0WDT();
        disableCore1WDT();
}

const char *format_digit(uint8_t digit) {
  digit = digit % 100;
  formatted_digit[0] = '0' + digit / 10;
  formatted_digit[1] = '0' + digit % 10;
  formatted_digit[2] = 0;
  return formatted_digit;
}

void print_date_and_time(const DateTime& date_and_time) {
  Serial.print(date_and_time.year());
  Serial.print('-');
  Serial.print(format_digit(date_and_time.month()));
  Serial.print('-');
  Serial.print(format_digit(date_and_time.day()));
  Serial.print(' ');
  Serial.print(format_digit(date_and_time.hour()));
  Serial.print(':');
  Serial.print(format_digit(date_and_time.minute()));
  Serial.print(':');
  Serial.print(format_digit(date_and_time.second()));
}

void setup() {
  Serial.begin(115200);

  Serial.println("DS3231 time initializer starting");

  setenv("TZ", "UTC", 1);
  tzset();

  Wire.begin();
  if (!time_keeper.begin()) {
    Serial.println("Could not locate DS3231");
    shutdown();
  }

  Serial.print("Connecting to WiFi");
  WiFi.begin("YOUR_NETWORK_ID", "YOUR_PASSWORD");
  wl_status_t wifi_connect_status;
  while ((wifi_connect_status = WiFi.status()) != WL_CONNECTED) {
    if (wifi_connect_status == WL_CONNECT_FAILED) {
      Serial.println("connection failed.");
      shutdown();
    }
    Serial.print(' ');
    Serial.print(wifi_connect_status);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  Serial.println();
  Serial.print("Connected as ");
  Serial.println(WiFi.localIP());

  time_client.begin();
  Serial.println("Time client started. Trying to fetch current time.");

  bool time_is_available = false;
  uint16_t number_of_calibrations = 0;
  while (!(time_is_available = time_client.update()) && (number_of_calibrations < 1000)) {
    time_client.forceUpdate();
    ++number_of_calibrations;
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  if (time_is_available) {
    Serial.println("Time client updated.");

    unsigned long epoch = time_client.getEpochTime();
    DateTime date_and_time(epoch);
    time_keeper.adjust(date_and_time);
    Serial.print("Epoch is: ");
    Serial.print(epoch);
    Serial.print(", and the unix time is: ");
    Serial.print(date_and_time.unixtime());
    Serial.print("Setting date and time to: ");
    Serial.print(date_and_time.timestamp());
    Serial.println();

    if (time_keeper.lostPower()) {
      Serial.println("Clock has stopped!");
    } else {
      Serial.println("Clock is running");
    }

    Serial.println("Fetching time from the DS3231");
    DateTime time_from_clock = time_keeper.now();
    timeval tv;
    tv.tv_sec = time_from_clock.unixtime();
    tv.tv_usec = 0;
    timezone tz_utc = {0, 0};
    settimeofday(&tv, &tz_utc);

    Serial.print("Date and time actually set to: ");
    Serial.println(time_from_clock.timestamp());

  } else {
    Serial.println("Could not fetch time, giving up.");
  }
}


// The loop function is called in an endless loop
void loop() {
	if (number_of_times++ < 20) {
		Serial.println(time_keeper.now().timestamp());
	}
	Serial.println();
	vTaskDelay(pdMS_TO_TICKS(1000));
}
