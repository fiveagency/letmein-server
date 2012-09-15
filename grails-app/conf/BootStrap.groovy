import eu.fiveminutes.lmi.Door
import eu.fiveminutes.lmi.Role
import eu.fiveminutes.lmi.User
import eu.fiveminutes.lmi.UserRole

class BootStrap {

    def init = { servletContext ->
        def door = new Door(pin: "3412").save(flush: true)

        def adminRole = new Role(authority: Role.ADMIN).save(flush: true)
        def userRole = new Role(authority: Role.USER).save(flush: true)
        def user = new User(username: 'admin', enabled: true, password: 'resetme').save(flush: true)
        UserRole.create user, adminRole, true

        assert Door.count() == 1
        assert Role.count() == 2
        assert User.count() == 1
        assert UserRole.count() == 1
    }
    def destroy = {
    }
}
