package eu.fiveminutes.lmi

import grails.plugins.springsecurity.Secured

@Secured([Role.ADMIN])
class AdminController {

    static allowedMethods = [update: "POST"]
    
    def springSecurityService
    def arduinoService

    def index() {
        redirect(action: "edit", params:params)
    }
    
    def edit() {
        def doors = Door.list(max:1)
        if (!doors) {
            log.error "No door in database!"
            flash.error = message(code: 'door.not.found.message', default: 'No door in database!')
            response.sendError(500, "Door not found in db!")
            return
        }

        [doorInstance: doors[0]]
    }
    
    def update(Long id, Long version) {
        def doorInstance = Door.get(id)
        if (!doorInstance) {
            log.error "No door in database!"
            flash.error = message(code: 'door.not.found.message', default: 'No door in database!')
            response.sendError(500, "Door not found in db!")
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

        if (params.password) {
            if (params.password != params.password2) {
                flash.error = message(code: 'error.password.dont.match.message', default: 'Passwords do not match')
                render(view: "edit", model: [doorInstance: doorInstance])
                return
            }
            def user = User.get(springSecurityService.currentUser.id)
            user.password = params.password
            if (!user.save(flush: true)) {
                flash.error = message(code: 'error.password.not.saved.message', default: 'Password not saved')
                render(view: "edit", model: [doorInstance: doorInstance])
                return
            }
        }
        
        def newPin = params.pin
        if (newPin != doorInstance.pin) {
            arduinoService.changePin(newPin)
            doorInstance.properties = params
            if (!doorInstance.save(flush: true)) {
                render(view: "edit", model: [doorInstance: doorInstance])
                return
            }
        }

        flash.message = message(code: 'settings.updated.message', default: 'Settings updated')
        redirect(action: "edit", id: doorInstance.id)
    }
}
