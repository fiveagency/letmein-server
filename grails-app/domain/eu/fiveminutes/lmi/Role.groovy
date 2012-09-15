package eu.fiveminutes.lmi

class Role {

    public static final String USER = "ROLE_USER";
    public static final String ADMIN = "ROLE_ADMIN";

    String authority

    static mapping = { cache true }

    static constraints = {
        authority blank: false, unique: true
    }
}
