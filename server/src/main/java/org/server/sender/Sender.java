package org.server.sender;

import org.server.handle.Handler;
import org.server.message.Message;
import java.io.OutputStream;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;

public class Sender implements Runnable{

    OutputStream out;
    LinkedBlockingQueue<byte[]> taskQueue;

    public Sender(OutputStream out, LinkedBlockingQueue<byte[]> taskQueue)
    {
        this.out = out;
        this.taskQueue = taskQueue;
    }

    @Override
    public void run()
    {

        while(true)
        {
            try
            {

                byte[] messageToSend = taskQueue.take();
                if(messageToSend != null)
                {
                    System.out.println("[+] Sending response to agent...");
                    this.out.write(messageToSend);
                }
                else
                {
                    System.out.println("[-] An error occurred with the response creation...");
                    this.out.close();
                }
            }
            catch (Exception e)
            {
                e.printStackTrace();
                System.exit(1);
            }
        }

    }
}
