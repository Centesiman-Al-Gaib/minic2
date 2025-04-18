package org.server.sender;

import org.server.handle.Handler;
import org.server.message.Message;
import java.io.OutputStream;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;

public class Sender implements Runnable{

    OutputStream out;
    LinkedBlockingQueue<Message> taskQueue;

    public Sender(OutputStream out, LinkedBlockingQueue<Message> taskQueue)
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

                Message messageToSend = taskQueue.take();
                Handler handler = Handler.getHandle(messageToSend);
                byte[] response = handler.handle();
                if(response != null)
                {
                    System.out.println("[+] Sending response to agent...");
                    this.out.write(response);
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
