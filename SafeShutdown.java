import com.pi4j.io.serial.*;
import com.pi4j.util.*;
import java.text.*;
import java.util.*;
import java.io.*;
import java.lang.*;
public class SafeShutdown extends Thread {
    @Override
    public void run () {
        //ArduinoComm pwMonitor = new ArduinoComm(args);
        try {
            String[] args = new String[] {"/bin/bash", "-c", "python /home/pi/PwMeterIndepBranch/sendLog.py"};
            Process proc = new ProcessBuilder(args).start();
            BufferedWriter writer = new BufferedWriter(new FileWriter(ArduinoComm.measurementSharing));
            writer.write("off");
            writer.close();
            ArduinoComm.fileWriter.close();
            ArduinoComm.bufferedWriter.close();
        }
        catch (IOException ioe) {
            System.out.println("Could not close files, unsafe shutdown.");
            ioe.printStackTrace();
            System.exit(0);
        }
        System.out.println("\nFiles closed, safe shutdown is OK.");
    }
}
