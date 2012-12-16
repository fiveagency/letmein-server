import eu.fiveminutes.lmi.ArduinoHttpService

// Place your Spring DSL code here
beans = {
    //    arduinoService(ArduinoSerialService, '${arduino.serialPortName}'){
    //
    //    }
    arduinoService(ArduinoHttpService, '${arduino.ip}'){

    }
}
