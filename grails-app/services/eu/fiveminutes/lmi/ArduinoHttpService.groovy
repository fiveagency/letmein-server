package eu.fiveminutes.lmi

import groovyx.net.http.HTTPBuilder

class ArduinoHttpService {

    private String baseUrl

    def ArduinoHttpService(String ip) {
        log.info "Initializing"
        baseUrl = "http://" + ip
    }

    def sendOpenRequest() {
        log.info "Sending open command"
        def door = Door.list(max:1)[0]
        try {
            def http = new HTTPBuilder(baseUrl)
            def postBody = [pin:door.pin]
            http.post(path: "/letmein/door/open", body: postBody)
        }
        catch (Exception e) {
            log.error(e, e)
        }
    }

    def changePin(String newPin) {
        log.info "Sending change pin command"
        def door = Door.list(max:1)[0]
        try {
            def http = new HTTPBuilder(baseUrl)
            def postBody = [oldPin:door.pin, newPin:newPin]
            http.post(path: "/letmein/door/changePin", body: postBody)
        }
        catch (Exception e) {
            log.error(e, e)
        }
    }
}
