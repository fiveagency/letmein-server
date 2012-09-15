class UrlMappings {

    static mappings = {
        "/$controller/$action?/$id?"{ constraints { // apply constraints here
            } }

        "/"(controller:"door")
        "500"(view:'/error')
    }
}
