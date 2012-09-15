package eu.fiveminutes.lmi

import grails.plugins.springsecurity.Secured

class DoorController {

    static allowedMethods = [update: "POST", openAuth: "POST"]
    
    def doorService

    @Secured([Role.USER, Role.ADMIN])
    def index() {
    }
    
    @Secured([Role.USER, Role.ADMIN])
    def show(Long id) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            flash.message = message(code: 'default.not.found.message', args: [message(code: 'door.label', default: 'Door'), id])
            redirect(action: "list")
            return
        }

        [doorInstance: doorInstance]
    }

    @Secured([Role.USER, Role.ADMIN])
    def edit(Long id) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            flash.message = message(code: 'default.not.found.message', args: [message(code: 'door.label', default: 'Door'), id])
            redirect(action: "list")
            return
        }

        [doorInstance: doorInstance]
    }

    @Secured([Role.USER, Role.ADMIN])
    def update(Long id, Long version) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            flash.message = message(code: 'default.not.found.message', args: [message(code: 'door.label', default: 'Door'), id])
            redirect(action: "list")
            return
        }

        if (version != null) {
            if (doorInstance.version > version) {
                doorInstance.errors.rejectValue("version", "default.optimistic.locking.failure",
                          [message(code: 'door.label', default: 'Door')] as Object[],
                          "Another user has updated this Door while you were editing")
                render(view: "edit", model: [doorInstance: doorInstance])
                return
            }
        }

        doorInstance.properties = params

        if (!doorInstance.save(flush: true)) {
            render(view: "edit", model: [doorInstance: doorInstance])
            return
        }

        flash.message = message(code: 'default.updated.message', args: [message(code: 'door.label', default: 'Door'), doorInstance.id])
        redirect(action: "show", id: doorInstance.id)
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
