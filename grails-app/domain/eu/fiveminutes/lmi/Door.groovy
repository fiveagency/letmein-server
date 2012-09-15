package eu.fiveminutes.lmi

class Door {

    String pin

    static constraints = {
        pin blank: false, unique: true
    }
}
