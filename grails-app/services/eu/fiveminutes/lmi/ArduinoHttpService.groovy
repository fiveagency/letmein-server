package eu.fiveminutes.lmi

class ArduinoHttpService {

    private String ip
    private URL url

    def ArduinoHttpService(String ip) {
        log.info "Initializing"
        this.ip = ip
        String urlString = "http://" + ip + "/door/open"
        url = urlString.toURL();
    }

    def sendRequest() {
        url.text
    }
}
