package eu.fiveminutes.lmi

class Door {

    String pin

    static constraints = {
        pin blank: false, unique: true, matches: "[1-9]{4}"
    }

    static mapping = { cache true }
}
