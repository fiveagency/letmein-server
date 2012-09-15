package eu.fiveminutes.lmi


class DoorService {

    def serialPortService

    def open() {
        log.info "Opening door"
        serialPortService.sendData()
    }
}
