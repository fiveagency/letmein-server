import java.util.concurrent.LinkedBlockingQueue

import eu.fiveminutes.lmi.ArduinoHttpService
import eu.fiveminutes.lmi.DoorWorkerService

// Place your Spring DSL code here
beans = {
    openDoorQueue(LinkedBlockingQueue){

    }
    //    arduinoService(ArduinoSerialService, '${arduino.serialPortName}'){
    //
    //    }
    arduinoService(ArduinoHttpService, '${arduino.ip}'){

    }
    doorWorkerService(DoorWorkerService, openDoorQueue, arduinoService, '${doorWorker.delay}'){

    }
}
