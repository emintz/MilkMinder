# MilkMinder Milk Delivery Notification System

MilkMinder notifies its user when milk has been delivered. It consists of two
components.

1. A receiver, which sits on the milk container and notifies the
   receiver when the milk carrier raises its lid
2. A receiver which displays delivery status and sounds an
   alert when the milk arrives.

The system uses ESP32 S2 development boards which communicate via
ESP NOW. The sender incorporates a MCU6050 gyroscope to sense
tilt, and the receiver incorporates a DS3231 time source to determine delivery
time. The receiver also displays the current time and delivery status on a 2 row by
16 column LCD and sounds an alarm when the milk arrives. Any low power,
3.3 Volt alarm will do. 
