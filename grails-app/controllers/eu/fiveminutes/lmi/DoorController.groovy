package eu.fiveminutes.lmi

import grails.plugins.springsecurity.Secured

class DoorController {

    static allowedMethods = [open: "POST", openAuth: "POST", validate: "POST"]
    
    def arduinoService

    @Secured([Role.USER, Role.ADMIN])
    def index() {
    }
    
    @Secured([Role.USER, Role.ADMIN])
    def openAuth() {
        log.info "Received open request from web"
        arduinoService.sendOpenRequest()
        redirect(action: "index")
    }
    
    
    //remove once mobile clients are calling arduino directly
    def open(String pin) {
        log.info "Received open request from mobile"
        proccessPin(pin, true)
    }
    
    def validate(String pin) {
        log.info "Received validate request"
        proccessPin(pin, false)
    }
    
    private proccessPin(String pin, boolean openDoor) {
        def doors = Door.list(max:1)
        if (doors) {
            def door = doors[0]
            if (door.pin == pin) {
                log.info "Received correct PIN"
                if (openDoor) {
                    arduinoService.sendOpenRequest(pin)
                }
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
}
