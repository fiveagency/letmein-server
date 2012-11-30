package eu.fiveminutes.lmi

import gnu.io.SerialPort;

import gnu.io.CommPortIdentifier
import gnu.io.SerialPort

import java.util.concurrent.TimeUnit

class ArduinoSerialService {

    private static final int TIME_OUT = 2000
    private static final int DATA_RATE = 9600
    private static String OPEN_DOOR_COMMAND = "OPEN"

    private SerialPort serialPort
    private OutputStream output
    private String portName

    def ArduinoSerialService(String portName) {
        this.portName = portName
        initialize()
    }

    def sendRequest() {
        try {
            write()
        } catch (Exception e) {
            log.error("Error while writing to serial port, retrying", e)
            Thread.sleep(TimeUnit.SECONDS.toMillis(2))
            try {
                close()
                initialize()
                write()
            } catch (Exception e2) {
                log.error("Error while writing to serial port on second try, giving up", e2)
            }
        }
    }

    private def initialize() {
        log.info "Initializing"
        try {
            CommPortIdentifier portId = null
            Enumeration portEnum = CommPortIdentifier.getPortIdentifiers()

            // iterate through, looking for the port
            while (portEnum.hasMoreElements()) {
                CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement()
                if (currPortId.getName().equals(portName)) {
                    portId = currPortId
                    break
                }
            }

            if (portId == null) {
                log.error "Could not find COM port: $portName"
                return
            }

            // open serial port, and use class name for the appName.
            serialPort = (SerialPort) portId.open(this.getClass().getName(), TIME_OUT)

            // set port parameters
            serialPort.setSerialPortParams(DATA_RATE, SerialPort.DATABITS_8, SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE)

            output = serialPort.getOutputStream()
        } catch (Exception e) {
            log.error("Error while initializing serial port!", e)
        }
    }

    private def write() {
        log.info "Sending $OPEN_DOOR_COMMAND command"
        output.write(OPEN_DOOR_COMMAND.getBytes())
        log.info "Command sent"
    }

    /**
     * This should be called when you stop using the port. This will prevent port locking on platforms like Linux.
     */
    private def close() {
        log.info "Closing serial port"
        if (serialPort != null) {
            serialPort.close()
        }
    }
}
