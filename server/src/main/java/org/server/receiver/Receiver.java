package org.server.receiver;

import org.server.handle.OutputHandler;
import org.server.message.Message;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;

public class Receiver implements Runnable{

    private static final int MESSAGE_SIZE_FIELD_SIZE = 4;

    int agentId; // Agent ID associated with this receiver
    InputStream in; // Socket used for listening to new messages
    File output; // File to write agent result

    public Receiver(int agentId, InputStream in, String outputFileName)
    {
        this.agentId = agentId;
        this.in = in;
        this.output = new File(outputFileName);
        try {
            this.output.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(1);
        }
    }

    @Override
    public void run() {

        while(true)
        {
            try
            {
                /* READING MESSAGE TYPE */
                int type =  this.in.read();

                /* READING PAYLOAD SIZE */
                byte[] pSize = this.in.readNBytes(MESSAGE_SIZE_FIELD_SIZE);
                if(pSize.length == 0)
                {
                    System.out.println("[-] An error occurred with the payload size...");
                }
                ByteBuffer bb = ByteBuffer.wrap(pSize).order(ByteOrder.LITTLE_ENDIAN);
                int size = bb.getInt();

                /* READING PAYLOAD */
                byte[] payloadBytes = this.in.readNBytes(size);
                if(payloadBytes.length != size)
                {
                    System.out.println("[-] An error occurred with the payload...");
                }

                /* GET MESSAGE OBJECT */
                Message message = Message.parseMessage(type, size, payloadBytes);
                String payload = new String(message.getPayload(), StandardCharsets.UTF_8);
                OutputHandler.writeResult(this.agentId, this.output, payload);
            }
            catch (IOException e)
            {
                System.out.println("[-] An error occurred...");
                e.printStackTrace();
                System.exit(1);
            }

        }
        
    }

}
