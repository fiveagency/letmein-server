package eu.fiveminutes.lmi

class DoorWorkerService {

    private def openDoorQueue
    private def arduinoService
    private long delay
    private Date lastTimeDoorOpened = new Date()

    def DoorWorkerService(def openDoorQueue, def arduinoService, long delay) {
        this.openDoorQueue = openDoorQueue 
        this.arduinoService = arduinoService 
        this.delay = delay
    }

    def consumeOpenDoorRequests() {
        try {
            Date firstOpenDoorRequest = openDoorQueue.take()
            def openDoorRequests = []
            int numberOfRequests = openDoorQueue.drainTo(openDoorRequests) + 1
            openDoorRequests.add(firstOpenDoorRequest)
            log.info "Received $numberOfRequests requests"
            
            openDoorRequests = openDoorRequests.sort { a, b ->
                b <=> a
            }
            
            Date lastOpenDoorRequest = openDoorRequests.get(0)
            if (lastOpenDoorRequest.getTime() - lastTimeDoorOpened.getTime() > delay) {
                log.info "Last open door request: $lastOpenDoorRequest is more than $delay ms after last time door opened: $lastTimeDoorOpened. Opening door"
                arduinoService.sendRequest()
                lastTimeDoorOpened = new Date()
            }
            else {
                log.info "Last open door request: $lastOpenDoorRequest is less than $delay ms after last time door opened: $lastTimeDoorOpened. Ignoring"
            }
            
        }
        catch(Exception e) {
            log.error("Error consuming open door requests", e)
        }
    }
}
