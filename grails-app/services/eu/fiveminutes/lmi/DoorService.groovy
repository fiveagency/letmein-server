package eu.fiveminutes.lmi

class DoorService {

    def openDoorQueue

    def open() {
        try {
            log.info "Adding open door request to queue"
            openDoorQueue.put(1)
        }
        catch(Exception e) {
            log.error("Error adding open door request to queue", e)
        }
    }
}
