package eu.fiveminutes.lmi

import grails.converters.JSON

class DoorController {

    static allowedMethods = [save: "POST", update: "POST", delete: "POST"]

    def index() {
        params["id"]=1
        redirect(action: "show", params: params)
    }

    def show(Long id) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            flash.message = message(code: 'default.not.found.message', args: [message(code: 'door.label', default: 'Door'), id])
            redirect(action: "list")
            return
        }

        [doorInstance: doorInstance]
    }

    def edit(Long id) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            flash.message = message(code: 'default.not.found.message', args: [message(code: 'door.label', default: 'Door'), id])
            redirect(action: "list")
            return
        }

        [doorInstance: doorInstance]
    }

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
        def doorStatus = new DoorStatus()
        def doors = Door.list(max:1)
        if (doors) {
            def door = doors[0]
            if (door.pin == pin) {
                log.info "Received correct PIN"
                doorStatus.status = "OK"
            }
            else {
                log.info "Received incorrect PIN"
                doorStatus.status = "FAIL"
            }
        }
        else {
            log.error "No door in database!"
            doorStatus.status = "FAIL"
        }
        render doorStatus as JSON
    }
}
