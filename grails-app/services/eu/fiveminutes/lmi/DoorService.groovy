package eu.fiveminutes.lmi


class DoorService {

    def serialPortService

    def addOpenRequest() {
        log.info "Opening door"
        serialPortService.sendData()
    }
}
