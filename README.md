# ESP32 IoT Boilerplate

This project provides a simple starting template for developing IoT projects based on ESP32 chip. It is based on [PlatformIO](https://platformio.org/ "PlatformIO") (a platform for embedded development) and [ESP-IDF](https://docs.espressif.com/projects/esp-idf/ "ESP-IDF") (the official development framework for ESP32 systems by Espressif).

## Overview

The template contains the following  components:

- `src/wifi.c` handles connection to a WiFi network
- `src/http.c` provides a HTTP server
- `src/mdns.c` provides a mDNS service to advertise a hostname
- `src/led.c` gives an example of a task that controls connected peripherals
- `src/sensor.c` gives an example of a task that reads sensor data and publishes them through the HTTP server

The components are loosely coupled, meaning that communication between them does not involve direct method calls, but instead the components use ESP-IDF framework mechanisms to send events and notifications to each other. For eaxmple: after the chip connects to a WiFi network and obtains an IP address, a custom event is emitted and the HTTP component, which listens for this event, starts the HTTP server service.

The WiFi component tries to connect the chip to a wireless access point using the specified SSID and access password. When the connection is established, it sends `APP_EVENT_WIFI_CONNECTED` that other componnt may subscribe to. In case the connection to WiFi is lost, `APP_EVENT_WIFI_DISCONNECTED` event is emitted to the event loop and the component periodically tries to re-establish the connection in specified intervals.

The HTTP component starts a HTTP server that defines `/status` URI. A request to that URI calls a handler that constructs a JSON string containg data from the sensor.

The LED component is a simple demonstration of a task that drives an output pin of the ESP32 chip to control a LED diode. It has no dependencies on other components.

The SENSOR component reads data from a simulated sensor (temperature and humidity) in periodic intervals and stores the last reading in an internal variable. On an HTTP request, the last measured values are obtained from that variable and passed as a json-encoded response.



## Installation & Usage

If you are using PlatformIO IDE (e.g. within Visual Studio Code), then simply dowmload the project and open it using PlatformIO IDE's Open Project button.

You may need to edit `platformio.ini` file and set the type of the chip that matches the one you have. Reffer to [PlatformIO Board List](https://platformio.org/boards "PlatformIO Board List") to find the matching one.
```ini
[env:nodemcu-32s]
board = nodemcu-32s
```

### Configuration

Before you compile the project, you need to set some configuration variables (not all of them have default values). This is mainly the case for the name of the WiFi network the chip should connect to and the access password. Refer to `src/Kconfig.projbuild` for a list of configuration options.

The easiest way to set the configuration options is to run
```bash
$ pio run -t menuconfig
```
Then navigate to "Application Configuration" and set the listed options.

### Building and running

From PlatformIO IDE issue Build command and then Flash command to upload the compiled binary to your chip over USB. Then you may switch to a monitor console to observe logging information.

There is a 10s pause at the very beginning of `app_main()` function that allows you to switch to the monitor console before the code starts to doing anything. You can remove this delay for production.

Note: you may need to adjust baud rate in platformio.ini (`monitor_speed` option) to match the default serial transmission rate of your chip.


If you are using PlatformIO CLI, then you issue the following commands
```bash
# compile the project
$ pio run

# upload the compiled binary to esp32 board
$ pio run -t upload

# open serial console to monitor log outputs
$ pio device monitor
```

## Dependencies

The project is based on  [PlatformIO](https://platformio.org/ "PlatformIO") and you need to have either PlatformIO IDE or PlatformIO CLI to build it. All necessary components (such as ESP-IDF SDK) will be loaded by PlatformIO. The project uses no other libraries.


## TODO plan

* Over-the-Air (OTA) updates

## Contributing

Contributions are welcome.

## Licence

This project is released under MIT license.

