package eu.fiveminutes.lmi


class DoorService {

    def serialPortService

    synchronized def open() {
        log.info "Opening door"
        serialPortService.sendData()
    }
}
