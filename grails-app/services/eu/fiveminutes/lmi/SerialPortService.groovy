package eu.fiveminutes.lmi

import gnu.io.SerialPort;

import gnu.io.CommPortIdentifier
import gnu.io.SerialPort

class SerialPortService {

    private static final int TIME_OUT = 2000
    private static final int DATA_RATE = 9600

    private SerialPort serialPort
    private OutputStream output
    private int signalDuration
    private String portName

    def SerialPortService(String portName, int signalDuration) {
        this.portName = portName
        this.signalDuration = signalDuration
        initialize();
    }

    private def initialize() {
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
                log.error "Could not find COM port."
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
        log.info "Sending duration: " + signalDuration
        output.write(String.valueOf(signalDuration).getBytes())
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

    def sendData() {
        try {
            write()
        } catch (Exception e) {
            log.error("Error while writing to serial port, retrying", e)
            try {
                close()
                initialize()
                write()
            } catch (Exception e2) {
                log.error("Error while writing to serial port on second try, giving up", e2)
            }
        }

    }
}
