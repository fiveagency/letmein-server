package eu.fiveminutes.lmi

import grails.plugins.springsecurity.Secured

class DoorController {

    static allowedMethods = [open: "POST", openAuth: "POST"]
    
    def doorService

    @Secured([Role.USER, Role.ADMIN])
    def index() {
    }

    def open(String pin) {
        def doors = Door.list(max:1)
        if (doors) {
            def door = doors[0]
            if (door.pin == pin) {
                log.info "Received correct PIN"
                doorService.open()
                response.sendError(200)
            }
            else {
                log.info "Received incorrect PIN"
                response.sendError(401, "Incorrect PIN")
            }
        }
        else {
            log.error "No door in database!"
            response.sendError(404, "Door not found in db!")
        }
    }
    
    @Secured([Role.USER, Role.ADMIN])
    def openAuth() {
        doorService.open()
        redirect(action: "index")
    }
}
