package eu.fiveminutes.lmi

class DoorWorkerService {

    def openDoorQueue
    def serialPortService

    def consumeOpenDoorRequests() {
        try {
            openDoorQueue.take()
            int numberOfRequests = openDoorQueue.drainTo(new ArrayList<Object>()) + 1
            log.info "Received $numberOfRequests requests. Opening door"
            serialPortService.sendData()
        }
        catch(Exception e) {
            log.error("Error consuming open door requests", e)
        }
    }
}
