package eu.fiveminutes.lmi

import eu.fiveminutes.lmi.arduino.ArduinoControl

class DoorService {

    private ArduinoControl arduinoControl = new ArduinoControl()

    def open() {
        log.info "Opening door"
        arduinoControl.open()
    }
}
