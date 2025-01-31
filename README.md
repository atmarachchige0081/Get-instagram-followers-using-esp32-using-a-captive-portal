# ESP32 Captive Portal with Instagram Follower Fetch

This project provides an ESP32-based captive portal that prompts a user for Wi-Fi credentials and an Instagram username. Upon successful configuration, the ESP32 connects to the specified Wi-Fi and periodically fetches the Instagram follower count for the provided username.

## Overview

### Captive Portal: When powered on and not configured, the ESP32 starts as a Wi-Fi Access Point (AP) and redirects any connected client to a webpage.
Credentials Collection: Users can enter their Wi-Fi SSID, Wi-Fi Password, and Instagram Username in the portal.

### Network Connection: Once the form is submitted, the ESP32 attempts to connect to the provided Wi-Fi network.

### Periodic Follower Retrieval: While connected to Wi-Fi, the device calls Instagram’s API to retrieve the follower count and prints it to the serial console.
Required Libraries and Dependencies

Before compiling and uploading the code, ensure the following libraries and dependencies are installed in your Arduino IDE (or equivalent environment):

## ESP32 Board Package

### Install via Arduino IDE:

Tools → Board → Boards Manager...
Search for ESP32 and install the official Espressif package.
DNSServer

Usually included with the ESP32 board package. Check by searching within the Library Manager if needed.
WebServer

Also typically part of the ESP32 core libraries. Confirm it is installed along with the ESP32 board package.
WiFiClientSecure

This is included in the ESP32 core libraries, providing SSL/TLS support for HTTPS connections.
ArduinoJson (version 6+)

## Install via Arduino IDE:

Sketch → Include Library → Manage Libraries...
Search for ArduinoJson and install the latest stable release.
Hardware Requirements
ESP32 Development Board (e.g., ESP32-DevKitC, NodeMCU-32S, etc.).
USB cable to connect the ESP32 board to your computer.
Setup Instructions
Clone or Download the Repository

Obtain the project files and open them in the Arduino IDE.
Open the Project

In Arduino IDE, go to File → Open... and select the .ino file. Ensure CaptivePortalManager.h and CaptivePortalManager.cpp are in the same directory.
Select the Correct Board

Go to Tools → Board → select your ESP32 board (e.g., “ESP32 Dev Module”).
Select the Correct Port

Under Tools → Port, choose the port where your ESP32 is connected.
Verify/Compile

Click the checkmark button in the Arduino IDE toolbar to verify that the code compiles without errors.
Upload

Click the arrow (Upload) button in the IDE toolbar. The code will be compiled and flashed onto your ESP32 board.
Open Serial Monitor (Optional but recommended)

Go to Tools → Serial Monitor.

Set the baud rate to 115200.
You will see debug and status messages related to the captive portal and Instagram data retrieval.
Usage

## After Upload

The ESP32 will restart and create an Access Point (default name: ConfigPortal with no password).
Check your Serial Monitor to see messages indicating the AP is active.
Connect to the ESP32 AP

Using your laptop (or any Wi-Fi capable device), connect to the ConfigPortal network.
Captive Portal

Once connected, open any web page in your browser. The captive portal should redirect you to the configuration page automatically.
If the portal does not appear, manually navigate to http://192.168.4.1/ in your browser.
Enter Credentials

### On the webpage, enter:

Wi-Fi SSID
Wi-Fi Password
Instagram Username
Click Connect.
Monitoring the Connection

The ESP32 will attempt to connect to your Wi-Fi. Check the Serial Monitor for progress.

If successful, the ESP32 will retrieve and print the Instagram follower count periodically (default every 60 seconds).
Additional Notes.

If your Wi-Fi credentials are invalid, the device may fail to connect. It will remain in station mode. In such cases, you may need to reset or re-flash the ESP32 to re-enter the captive portal sequence.
HTTPS Certificate Validity: The code sets the WiFiClientSecure client to insecure mode for simplicity. For production-grade security, provide a valid certificate fingerprint or root CA.

## Running on a Laptop

Ensure Wi-Fi is enabled on your laptop and that you can connect to new wireless networks.
Select “ConfigPortal” in your available Wi-Fi networks.

Open a browser, and you should be redirected automatically. If not, navigate to http://192.168.4.1.
Troubleshooting

No Automatic Captive Portal: Manually navigate to http://192.168.4.1.

Connection Timeouts: Check your Wi-Fi credentials, and ensure the access point is still broadcasting.
Instagram API Issues:

The Instagram endpoint may change. Verify the endpoint is still valid if you encounter parsing errors.
Ensure your ArduinoJson library is up to date.

## License:
You may include any specific licensing terms here (e.g., MIT, GPL, etc.) if relevant for your distribution.

## Author/Contact:

For questions or feedback, add your contact info or GitHub profile here if desired.
