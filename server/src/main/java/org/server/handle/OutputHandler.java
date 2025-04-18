package org.server.handle;

import org.server.message.Message;
import org.server.message.MessageType;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Random;

public class OutputHandler
{

    public static synchronized void writeResult(int agentId, File output, String payload)
    {
        try (FileWriter fw = new FileWriter(output, true))
        {
            fw.write(payload);
            fw.write("\n");
        }
        catch (IOException e)
        {
            System.out.println("[-] An error occurred.");
            e.printStackTrace();
            System.exit(1);
        }
    }


}
