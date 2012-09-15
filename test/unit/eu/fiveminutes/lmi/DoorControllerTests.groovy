package eu.fiveminutes.lmi



import org.junit.*
import grails.test.mixin.*

@TestFor(DoorController)
@Mock(Door)
class DoorControllerTests {

    def populateValidParams(params) {
        assert params != null
        // TODO: Populate valid properties like...
        //params["name"] = 'someValidName'
    }

    void testIndex() {
        controller.index()
        assert "/door/list" == response.redirectedUrl
    }

    void testList() {

        def model = controller.list()

        assert model.doorInstanceList.size() == 0
        assert model.doorInstanceTotal == 0
    }

    void testCreate() {
        def model = controller.create()

        assert model.doorInstance != null
    }

    void testSave() {
        controller.save()

        assert model.doorInstance != null
        assert view == '/door/create'

        response.reset()

        populateValidParams(params)
        controller.save()

        assert response.redirectedUrl == '/door/show/1'
        assert controller.flash.message != null
        assert Door.count() == 1
    }

    void testShow() {
        controller.show()

        assert flash.message != null
        assert response.redirectedUrl == '/door/list'

        populateValidParams(params)
        def door = new Door(params)

        assert door.save() != null

        params.id = door.id

        def model = controller.show()

        assert model.doorInstance == door
    }

    void testEdit() {
        controller.edit()

        assert flash.message != null
        assert response.redirectedUrl == '/door/list'

        populateValidParams(params)
        def door = new Door(params)

        assert door.save() != null

        params.id = door.id

        def model = controller.edit()

        assert model.doorInstance == door
    }

    void testUpdate() {
        controller.update()

        assert flash.message != null
        assert response.redirectedUrl == '/door/list'

        response.reset()

        populateValidParams(params)
        def door = new Door(params)

        assert door.save() != null

        // test invalid parameters in update
        params.id = door.id
        //TODO: add invalid values to params object

        controller.update()

        assert view == "/door/edit"
        assert model.doorInstance != null

        door.clearErrors()

        populateValidParams(params)
        controller.update()

        assert response.redirectedUrl == "/door/show/$door.id"
        assert flash.message != null

        //test outdated version number
        response.reset()
        door.clearErrors()

        populateValidParams(params)
        params.id = door.id
        params.version = -1
        controller.update()

        assert view == "/door/edit"
        assert model.doorInstance != null
        assert model.doorInstance.errors.getFieldError('version')
        assert flash.message != null
    }

    void testDelete() {
        controller.delete()
        assert flash.message != null
        assert response.redirectedUrl == '/door/list'

        response.reset()

        populateValidParams(params)
        def door = new Door(params)

        assert door.save() != null
        assert Door.count() == 1

        params.id = door.id

        controller.delete()

        assert Door.count() == 0
        assert Door.get(door.id) == null
        assert response.redirectedUrl == '/door/list'
    }
}
