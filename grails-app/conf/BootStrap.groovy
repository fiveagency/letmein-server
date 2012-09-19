import eu.fiveminutes.lmi.Door
import eu.fiveminutes.lmi.Role
import eu.fiveminutes.lmi.User
import eu.fiveminutes.lmi.UserRole

class BootStrap {

    def init = { servletContext ->

        if (Door.count() == 0) {
            def door = new Door(pin: "1234").save(flush: true)
            assert Door.count() == 1
        }

        if (User.count() == 0) {
            def userRole = new Role(authority: Role.USER).save(flush: true)
            def adminRole = new Role(authority: Role.ADMIN).save(flush: true)
            def user = new User(username: 'admin', enabled: true, password: 'resetme').save(flush: true)
            UserRole.create user, adminRole, true
            def user2 = new User(username: 'marvin', enabled: true, password: 'resetme').save(flush: true)
            UserRole.create user2, adminRole, true
            def user3 = new User(username: '5min', enabled: true, password: '5min').save(flush: true)
            UserRole.create user3, userRole, true

            assert Role.count() == 2
            assert User.count() == 3
            assert UserRole.count() == 3
        }
    }
    def destroy = {
    }
}
