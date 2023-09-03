## STM32 Weather Station ![OpenWeather Logo](https://raw.githubusercontent.com/PUrban96/Weather-Station/master/Weather_Icon/10d%402x.png)


## Feature 
* **Clock**
* **Data**
* **Indoor conditions (real measurement):**
    - **Temperature**
    - **Humidity**
    - **Pressure**
* **Outdoor conditions (form OpenWeather API):**
    - **Temperature**
    - **Humidity**
    - **Pressure**
    - **Rain**
    - **Air quality index**
    - **Weather Icon**
    - **Wind Speed**
    - **Wind direction**
    - **Sunset/sunrise time**
    - **Visibility**
    - **Cloudiness**
* **Weather forecast for 3 days**
* **Other:**
	- **Auto LCD Brightness control**
	- **Configuration via simple website**
	- **Read weather icon form SDCard**

## Interface

<p align="left">
  <img src="https://raw.githubusercontent.com/PUrban96/Weather-Station/master/Foto_Device/20230617_163019.jpg" width="350" title="NormalMode">
  <img src="https://raw.githubusercontent.com/PUrban96/Weather-Station/master/Foto_Device/20230617_163010.jpg" width="400" alt="ConfigMode">
</p>

## Hardware 
* **Main CPU: STM32G070RBT6**
* **Indoor sensor: BMP280**
* **WiFi module: ESP8266-01**
* **LCD: 2.8inch (ILI9341 controler)**
* **Light sensor: 10k photoresistor**
* **DC/DC switcher: ST1S10PHR**
* **RTC battery: CR2032**
* **SDCard**

<p align="left">
  <img src="https://raw.githubusercontent.com/PUrban96/Weather-Station/master/Foto_Hardware/PCB_BOTTOM.jpg" width="350" title="NormalMode">
  <img src="https://raw.githubusercontent.com/PUrban96/Weather-Station/master/Foto_Hardware/PCB_TOP.jpg" width="350" alt="ConfigMode">
</p>


## Configuration
* **Copy the contents of the folder "Weather_Icon" to the SD card**
* **Click config buttor**
* **Connect with weather station access point (via Wi-Fi)**
* **Enter ID address in your browser**
* **Set your configuration:**
* **Wi-Fi SSID**
* **Wi-Fi password**
* **Weather Station API key (you must create account in https://openweathermap.org)**
    - **City**
	- **Latitude**
	- **Longitude**
	- **Date and time**
* **Click "Save" button and click config button**

<p align="left">
  <img src="https://raw.githubusercontent.com/PUrban96/Weather-Station/master/Foto_Interface/CONFIG_PAGE.jpg" width="350" title="hover text">
</p>

## Licence

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)