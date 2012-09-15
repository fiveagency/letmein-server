import eu.fiveminutes.lmi.Door
import eu.fiveminutes.lmi.DoorStatus
import grails.converters.JSON

class BootStrap {

    def init = { servletContext ->
        JSON.registerObjectMarshaller(DoorStatus) {DoorStatus it ->
            def returnArray = [:]
            returnArray['status'] = it.status
            return returnArray
        }

        def door = new Door(pin: "1234").save(flush: true)
        assert Door.count() == 1
    }
    def destroy = {
    }
}
