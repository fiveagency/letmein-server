package eu.fiveminutes.lmi

import grails.plugins.springsecurity.Secured

@Secured([Role.ADMIN])
class AdminController {

    static allowedMethods = [update: "POST"]

    def index() {
        params["id"] = 1
        redirect(action: "show", params:params)
    }
    
    def show(Long id) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            flash.message = message(code: 'default.not.found.message', args: [message(code: 'door.label', default: 'Door'), id])
            redirect(controller: "door")
            return
        }

        [doorInstance: doorInstance]
    }
    
    def edit(Long id) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            flash.message = message(code: 'default.not.found.message', args: [message(code: 'door.label', default: 'Door'), id])
            redirect(controller: "door")
            return
        }

        [doorInstance: doorInstance]
    }
    
    def update(Long id, Long version) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            flash.message = message(code: 'default.not.found.message', args: [message(code: 'door.label', default: 'Door'), id])
            redirect(controller: "door")
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
}
