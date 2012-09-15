import eu.fiveminutes.lmi.Door

class BootStrap {

    def init = { servletContext ->
        def door = new Door(pin: "1234").save(flush: true)
        assert Door.count() == 1
    }
    def destroy = {
    }
}
