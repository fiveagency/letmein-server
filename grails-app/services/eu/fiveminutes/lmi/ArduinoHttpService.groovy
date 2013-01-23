package eu.fiveminutes.lmi

import groovyx.net.http.HTTPBuilder

class ArduinoHttpService {

    private String baseUrl

    def ArduinoHttpService(String ip) {
        log.info "Initializing"
        baseUrl = "http://" + ip
    }

    def sendOpenRequest() {
        def door = Door.list(max:1)[0]
        sendOpenRequest(door.pin)
    }

    def sendOpenRequest(String pin) {
        log.info "Sending open command"
        try {
            def http = new HTTPBuilder(baseUrl)
            def postBody = [pin:pin]
            http.post(path: "/letmein/door/open", body: postBody)
            log.info "Sent open command"
        }
        catch (Exception e) {
            log.error("Error while sending open command", e)
        }
    }

    def changePin(String newPin) {
        log.info "Sending change pin command"
        def door = Door.list(max:1)[0]
        try {
            def http = new HTTPBuilder(baseUrl)
            def postBody = [oldPin:door.pin, newPin:newPin]
            http.post(path: "/letmein/door/changePin", body: postBody)
            log.info "Sent change pin command"
        }
        catch (Exception e) {
            log.error("Error while sending change pin command", e)
        }
    }
}
