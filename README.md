Server app for LetMeIn project
==============================
Controls electric door strike via Arduino.
Application has web UI that has open door button and admin page for changing door pin.
Application also has REST API used by mobile LetMeIn clients for sending open door signal to Arduino and verifying pin.

Hardware
--------
Arduino controls a door that has electric door strike with its own power supply. To open the door electric circuit needs to be closed for a short interval (currently 250 ms).
Relay is used to close the electric strike circuit. Relay is connected to Arduino using this schema: http://www.Arduino.cc/playground/uploads/Learning/relays.pdf
Components used are:
* F30229-12 relay
* 2N2222A transistor
* 1N4007 diode
* 1k resistor

Server app connects to Arduino via USB cable and uses rxtx library for serial communication.
Arduino code and dll used for serial communication are located in /arduino

Todo:
-----
* replace Arduino-server serial connection with Ethernet connection. ENC28J60 network module will be connected to Arduino and https://github.com/jcw/ethercard will be used to receive http requests.
* use secret knock to open the door. This will be done using piezo element placed on the door. Communication between the door and Arduino will be done using two eZ430-RF2500 wireless boards. One will be connected to piezo element on the door. Other will be connected to Arduino or possibly to the server app
* replace electric circuit described above with opto isolated relay board
