import eu.fiveminutes.lmi.SerialPortService

// Place your Spring DSL code here
beans = {
    serialPortService(SerialPortService, '${serialPortService.portName}', '${serialPortService.signalDuration}'){

    }
}
