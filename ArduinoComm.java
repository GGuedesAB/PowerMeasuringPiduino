import com.pi4j.io.serial.*;
import com.pi4j.util.CommandArgumentParser;
import com.pi4j.util.Console;
import java.text.*;
import java.util.*;
import java.io.*;
import java.lang.*;
public class ArduinoComm {
    /**
     * This example program supports the following optional command arguments/options:
     * Default device path is for GPIOs, if using USB change to ttyACM0
     *
     *   "--device (device-path)"                   [DEFAULT: /dev/ttyS0]
     *   "--baud (baud-rate)"                       [DEFAULT: 38400]
     *   "--data-bits (5|6|7|8)"                    [DEFAULT: 8]
     *   "--parity (none|odd|even)"                 [DEFAULT: none]
     *   "--stop-bits (1|2)"                        [DEFAULT: 1]
     *   "--flow-control (none|hardware|software)"  [DEFAULT: none]
     *
     * @param args
     * @throws InterruptedException
     * @throws IOException
     **/

    private final float alpha = 0.6f ;
    private static String instantPower;
    private static String devicePath;
    public static float cast = 0;
    public static float lowPass = 0;
    private static boolean newLogExists = false;
    private static String logName = "Blank";
    public static String measurementSharing = ".net_measurement.txt";
    public static FileWriter fileWriter;
    public static BufferedWriter bufferedWriter;
    private File file;
    //public static boolean running = true;
    public static ArduinoComm instance = null;

    private ArduinoComm (String args[]) throws InterruptedException, IOException {
	    //Default port is ttyS0
            devicePath = SerialPort.getDefaultPort();
            final Serial serial = SerialFactory.createInstance();
	    //Create and register the serial data listener
	    serial.addListener(new SerialDataEventListener() {
            @Override
            public void dataReceived(SerialDataEvent event) {
                try {
                    instantPower = event.getAsciiString();
                }
		        catch (IOException e) {
                    System.out.println("ENERGY MEASUREMENT:    Failed to read from measurement system.");
                    e.printStackTrace();
	            }
	        }
	    });
	    try {
            //Create serial config object
            SerialConfig config = new SerialConfig();
            if (config == null) {
                System.out.println("ENERGY MEASUREMENT:    Error reading serial port.");
                return;
            }
            //System.out.println("Default serial port is: "+SerialPort.getDefaultPort());
            //delay(5000);
            //Set default serial settings (device, baud rate, flow control, etc)
            config.device(devicePath)
                .baud(Baud._38400)
                .dataBits(DataBits._8)
                .parity(Parity.NONE)
                .stopBits(StopBits._1)
                .flowControl(FlowControl.NONE);

            //Parse optional command argument options to override the default serial settings.
            if(args.length > 0){
                config = CommandArgumentParser.getSerialConfig(config, args);
	        }
            //Open the default serial device/port with the configuration settings
            serial.open(config);
        }
        catch(Exception ex) {
            System.out.println(" ==>> SERIAL SETUP FAILED : " + ex.getMessage());
            return;
        }
    }

    public static ArduinoComm getInstance(String args[]) throws InterruptedException, IOException {
        if (instance == null) {
	    instance = new ArduinoComm(args);
	}
	    return instance;
    }

    private void delay (int time) {
        try {
            Thread.sleep(time);
        }
        catch (InterruptedException ie) {
            System.out.println("Cannot sleep.");
            ie.printStackTrace();
        }
    }

    public void createLog () {
        Scanner keyboard = new Scanner (System.in);
        Scanner fileName = new Scanner (System.in);
        System.out.println("***Starting measurement system***");
        System.out.println("First step is to create log file.");
        System.out.print("Create new log file? (Y/n) ");
        String response = keyboard.nextLine();
        if (response.equalsIgnoreCase("n")) {
            System.out.println("Using standard log file.");
        }
        else {
            System.out.println("Creating new log file, enter log file name: ");
            response = fileName.nextLine();
            logName = response+".txt";
        }
        System.out.println("Log file name is: "+logName);
        delay(3000);
        return;
    }

    public void startMeasurement () {
        float energy = 0;
        int i = 0;
        int j = 0;
        System.out.println("\033[H\033[2J");
        while (instantPower == null || instantPower == "" || instantPower == " ") {
            delay(500);
        }
        while(true) {
            if (instantPower != null && instantPower != "" && instantPower != " ") {
                //Getting time information and formating
                DateFormat dateFormat = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss");
                TimeZone.setDefault(TimeZone.getTimeZone("GMT-3:00"));
                Date isoFormat = new Date();
                Calendar cal = Calendar.getInstance();
                cal.setTime(isoFormat);
                //Casting types, turning string into float
                cast = Float.parseFloat(instantPower);
                lowPass = lowPass + alpha*(cast - lowPass);
                energy = energy + lowPass;
                NumberFormat formatter = new DecimalFormat("00");
                //Print information
                System.out.print(dateFormat.format(isoFormat)+": ");
                System.out.print("Instant Power (W): "+lowPass+"   ");
                System.out.print("Energy used (J): "+energy);
                //Writing to log file and network sharing file
                try {
                    File netMeasurement = new File (measurementSharing);
                    if (!netMeasurement.exists()) {
                        netMeasurement.createNewFile();
                    }
                    BufferedWriter writer = new BufferedWriter(new FileWriter(measurementSharing));
                    writer.write(lowPass+" "+energy);
                    writer.close();
                    int day = cal.get(Calendar.DAY_OF_MONTH);
                    //Months are indexed from 0 to 11 (WTF)
                    int month = cal.get(Calendar.MONTH)+1;
                    int year = cal.get(Calendar.YEAR);
                    String logDate = formatter.format(day)+"."+formatter.format(month)+"."+year;
                    String logName = "Measurement:"+logDate+".txt";
                    file = new File (logName);
                    if (!file.exists()) {
                        file.createNewFile();
                    }
                    fileWriter = new FileWriter(file.getAbsoluteFile(), true);
                    bufferedWriter = new BufferedWriter (fileWriter);
                    String pwr = Float.toString (lowPass);
                    String enrg  = Float.toString (energy);
                    bufferedWriter.write(dateFormat.format(isoFormat).toString()+" "+pwr+" "+enrg+"\n");
                }
                catch (IOException ioe) {
                    System.out.println("Something went wrong with file writing.");
                    ioe.printStackTrace();
                }
                finally {
                    try {
                        if (bufferedWriter != null) {
                            bufferedWriter.close();
                        }
                        if (fileWriter != null) {
                            fileWriter.close();
                        }
                    }
                    catch (IOException ioe) {
                        System.out.println("Something went wrong when closing file.");
                        ioe.printStackTrace();
                    }
                }
                delay(1000);
                for (j=0; j<200; j++){
                    System.out.print("\b");
                }
            }
            else {
                System.out.println("Something went wrong with serial communication, trying again.");
                delay(5000);
            }
        }
    }

    public static void main(String args[]) throws InterruptedException, IOException {
	    ArduinoComm powerMeter = ArduinoComm.getInstance (args);
            Runtime.getRuntime().addShutdownHook(new SafeShutdown());
	    //powerMeter.createLog();
	    powerMeter.startMeasurement();
	    return;
    }
}
