package eu.fiveminutes.lmi

import grails.plugins.springsecurity.Secured

class DoorController {

    static allowedMethods = [open: "POST"]
    
    def arduinoService

    @Secured([Role.USER, Role.ADMIN])
    def index() {
    }
    
    @Secured([Role.USER, Role.ADMIN])
    def open() {
        log.info "Received open request"
        arduinoService.sendOpenRequest()
        redirect(action: "index")
    }
}
