package org.server;

import org.server.handle.Handler;
import org.server.message.Message;
import org.server.receiver.Receiver;
import org.server.sender.Sender;
import org.server.taskManager.TaskManager;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.HashMap;
import java.util.Random;
import java.util.concurrent.LinkedBlockingQueue;

public class Server{

    ServerSocket serverSocket;


    public static Server CreateServer(int port)
    {
        try
        {
            return new Server(port);
        }
        catch (IOException e)
        {
            System.out.println(("An error occurred during server initialization..." + e.getMessage()));
            return null;
        }
    }

    private Server(int port) throws IOException
    {
        this.serverSocket = new ServerSocket(port);
    }

    public void runServer() throws IOException {
        HashMap<Integer, LinkedBlockingQueue<Message>> taskManagerQueue = new HashMap<>();
        TaskManager taskManager = new TaskManager(taskManagerQueue);
        Thread taskManagerThread = new Thread(taskManager);
        taskManagerThread.start();
        while(true)
        {

            Socket communicationWithClient = this.serverSocket.accept();
            OutputStream out = communicationWithClient.getOutputStream();
            InputStream in = communicationWithClient.getInputStream();
            Random rand = new Random();
            int agentId = (int) (((System.currentTimeMillis() / 1000) + rand.nextInt(100000)) % 100000);
            LinkedBlockingQueue<Message> queue = new LinkedBlockingQueue<>();
            taskManagerQueue.put(agentId, queue);

            Receiver recv  = new Receiver(agentId, in, "C:\\Users\\usuariolocal\\Desktop\\output\\output.txt");
            Sender sender = new Sender(out, queue);

            Thread recvThread = new Thread(recv);
            Thread senderThread = new Thread(sender);
            recvThread.start();
            senderThread.start();


        }

    }

}
