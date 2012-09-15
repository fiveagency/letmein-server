package eu.fiveminutes.lmi

class LoginTagLib {

    def springSecurityService

    def loginControl = {
        if (springSecurityService.isLoggedIn()) {
            out << """Hello ${springSecurityService.currentUser.username}"""
            out << """[${link(controller:"logout"){"Logout"}}]"""
        }
        else {
            out << """[${link(controller:"login"){"Login"}}]"""
        }
    }
}
