package eu.fiveminutes.lmi.arduino

import gnu.io.CommPortIdentifier
import gnu.io.SerialPort

import java.util.concurrent.TimeUnit



class ArduinoControl {

    private SerialPort serialPort
    /** The output stream to the port */
    private OutputStream output
    /** Milliseconds to block while waiting for port open */
    private static final int TIME_OUT = 2000
    /** Default bits per second for COM port. */
    private static final int DATA_RATE = 9600

    private void initialize() {
        CommPortIdentifier portId = null
        Enumeration portEnum = CommPortIdentifier.getPortIdentifiers()

        // iterate through, looking for the port
        while (portEnum.hasMoreElements()) {
            CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement()
            if (currPortId.getName().equals("COM6")) {
                portId = currPortId
                break
            }
        }

        if (portId == null) {
            System.out.println("Could not find COM port.")
            return
        }

        try {
            // open serial port, and use class name for the appName.
            serialPort = (SerialPort) portId.open(this.getClass().getName(), TIME_OUT)

            // set port parameters
            serialPort.setSerialPortParams(DATA_RATE, SerialPort.DATABITS_8, SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE)

            // open the streams
            output = serialPort.getOutputStream()
            Thread.sleep(TimeUnit.SECONDS.toMillis(1))
        } catch (Exception e) {
            System.err.println(e.toString())
        }
    }

    /**
     * This should be called when you stop using the port. This will prevent port locking on platforms like Linux.
     */
    private void close() {
        if (serialPort != null) {
            serialPort.close()
        }
    }

    private void writeByetes(int duration) {
        try {
            output.write(String.valueOf(duration).getBytes())
        } catch (Exception e) {
            System.err.println(e.toString())
        }
    }

    public void open() {
        int duration = 500
        initialize()
        System.out.println("Started")
        writeByetes(duration)
        Thread.sleep(duration + 1000)
        close()
        System.out.println("Closed")
    }
}
