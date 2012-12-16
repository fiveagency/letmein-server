LetMeIn project: Arduino code, web app and eZ430-RF2500 code
==============================
Controls electric door strike via reley connected to Arduino.
Arduino can receive open door commands via Ethernet, Serial port or RF.
Arduino has REST API used by mobile LetMeIn clients for sending open door signal and verifying pin. It also has changePin method used by Web app.
Web application has open door button and admin page for changing door pin.

Hardware
--------
Arduino controls a door that has electric door strike with its own power supply. To open the door electric circuit needs to be closed for a short interval (currently 250 ms).
Relay is used to close the electric strike circuit.
Opto isolated reley board is connected to Arduino:
* VCC - 5V
* GND - GND
* IN - Pin 4

Server app connects to Arduino via Ethernet. It can also connect via USB cable and uses rxtx library for serial communication.
Arduino code and dll used for serial communication are located in /arduino. rxtx dll should be copied to JRE bin folder if you want to use serial port.

Arduino is connected to ENC28J60 network module:
* VCC - 3.3V
* GND - GND
* SCK - Pin 13
* SO - Pin 12
* SI - Pin 11
* CS - Pin 8

https://github.com/jcw/ethercard is used to receive http requests.

Todo:
-----
* use secret knock to open the door. This will be done using piezo element placed on the door. Communication between the door and Arduino will be done using two eZ430-RF2500 wireless boards. One will be connected to piezo element on the door. Other will be connected to Arduino or possibly to the server app

Arduino is also connected to eZ430-RF2500 wireless board (receiver):
* VCC - 3.3V
* GND - GND
* P4.3 (Pin 8) - Pin 2

eZ430-RF2500 wireless board - sender (connected to piezo and mounted on door):
* GND - Piezo -
* A0 (Pin 3) - Piezo +