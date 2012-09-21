import java.util.concurrent.LinkedBlockingQueue

import eu.fiveminutes.lmi.DoorWorkerService
import eu.fiveminutes.lmi.SerialPortService

// Place your Spring DSL code here
beans = {
    openDoorQueue(LinkedBlockingQueue){

    }
    serialPortService(SerialPortService, '${serialPortService.portName}', '${serialPortService.signalDuration}'){

    }
    doorWorkerService(DoorWorkerService, openDoorQueue, serialPortService, '${doorWorker.delay}'){

    }
}
