# Smart Digital Clock with Gesture-Controlled UI

A gesture-controlled digital clock built using Arduino Uno, designed to offer a fully touchless interface for time display, alarm setting, and environmental monitoring. This project demonstrates the practical application of sensor fusion and embedded systems in smart home devices.

## Features

* **Gesture-Based Alarm Setting**
  Set the alarm time using intuitive hand gestures via the APDS9960 sensor, eliminating the need for physical buttons.

* **Multi-View OLED Display**
  Seamlessly switch between different display modes—Time & Date, Temperature & Humidity, and Alarm Settings—using an ultrasonic sensor for circular view navigation.

* **Environmental Monitoring**
  Displays real-time ambient temperature and humidity using the DHT22 sensor.

* **Audible Feedback**
  A passive buzzer signals the alarm and provides feedback for successful actions like setting time or switching views.

## Hardware Used

* Arduino Uno
* APDS9960 Gesture Sensor
* Ultrasonic Sensor (for view switching)
* DHT22 Temperature & Humidity Sensor
* DS3231 RTC Module
* OLED Display (128x32 I2C)
* Passive Buzzer

## Software Details

* Developed in C++ using the Arduino framework.
* I2C communication used for OLED, RTC, and gesture sensors.
* Timekeeping handled by DS3231 RTC with backup battery support.
* Gesture recognition calibrated for proximity and gesture consistency.
* Alarm set via gestures: one phase for minutes, one for hours.

## Project Impact

* Achieved 100% touchless interaction.
* Enabled alarm setting up to 3× faster and navigation 2× faster compared to traditional digital clocks with physical buttons.
* Demonstrated practical use of embedded systems and sensor integration for smarter and more accessible user interfaces.

## Team

* **Yash Dongaonkar** – *Team Leader & Software Developer*
* Divyansh Agarwal
* Atharva Mhatre
* Rohit Singh Naruka
* Guided by **Dr. K. Surender**, VNIT Nagpur
