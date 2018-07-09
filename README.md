# Okuloj

<p align="center"><img src="https://raw.githubusercontent.com/0x19c0/Okuloj/master/okuloj.png" width="200"></p>

Okuloj is an ESP8266-based MAC address sniffer and logger. Once deployed, it continuously logs the identifiers of all nearby devices. You can then:  
  * Check the amount of devices at a given time.
  * Find when a specific device was nearby.
  * Deploy several chips in a grid and track everyone's movement in the area.
  * Exploit someone's known movement patterns to find out their MAC.  

The possibilities are vast. The ethics are questionable.  

This project was carried out as a part of "Advanced Programming Project" course at the Technion, under guidance of Dr. Shachar Itzhaky.

## High Level Overview

<p align="center"><img src="https://raw.githubusercontent.com/0x19c0/Okuloj/master/overview.png"></p>

The system consists of the following elements:
  * ESP8266 programmed with Arduino. The chip runs in promiscuous mode for some time, sniffing packets on a give channel and logging MAC addresses. It then goes online and flushes the logged data to Google App via GET requests. These two actions are performed indefinitely.
  * Google Sheet containing the logged data. Every line contains timestamp of the upload, the MAC address logged and the name of the uploading ESP. This sheet is not accessed directly, both ESP uploads and user requests for data are done through the corresponding App.
  * Google App processing requests by ESP and frontend. ESP requests are appends to the Sheet, frontend requests are answered with filtered Sheet data (by timestamp, ESP name, MAC etc.).
  * Frontend is a webpage that translates human-readable requests to GET requests, sends them to the App and shows the response once received. Mind that the terminal-like look is purely cosmetic.

## Dependencies

The process of building ESP8266 Arduino project is well described here:  
https://github.com/spacehuhn/esp8266_deauther/wiki/Installation#compiling-using-arduino-ide

This library is necessary for uploading the logs via Google Script:  
https://github.com/electronicsguy/ESP8266/tree/master/HTTPSRedirect

## User Guide

  * On your Google Drive, create a new Google Sheet.
  * Use Tools/Script editor, paste the okuloj.js file there. On the second line of this file you have to add the Sheet ID - copy it from URL of the Sheet (the part after /d/).
  * Publish/Deploy as web app. Give the access to the app to "Anyone, even anonymous". You will get the app URL - copy the Script ID.
  * Open okuloj.ino file and edit the following variables: kChannel (choose the channel used by a target WiFi), kSSID/kPassword (you need some access to the internet for uploading the data, for open networks leave the kPassword empty), kScriptID and kDeviceName (to distinguish between uploads of different ESPs if you've got multiple). Don't change other parameters if you don't have a clear idea what you're doing.
  * Compile and upload the program to the ESP. Deploy it at the target location.
  * In the index.html file, paste the Script ID into addrBase variable. Open the file using whatever browser.  

That's pretty much it.