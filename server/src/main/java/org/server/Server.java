package org.server;

import org.server.message.Message;
import org.server.message.MessageType;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Random;

public class Server{

    ServerSocket serverSocket;
    int MESSAGE_SIZE_FIELD_SIZE  = 4;

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
        while(true)
        {
            try
            {
                Socket communicationWithClient = this.serverSocket.accept();
                OutputStream out = communicationWithClient.getOutputStream();
                InputStream in = communicationWithClient.getInputStream();
                System.out.println("[+] A client has connected...");

                // Reading message type
                byte type = (byte) in.read();

                //Reading message payload size
                byte[] pSize = in.readNBytes(MESSAGE_SIZE_FIELD_SIZE);
                if(pSize.length == 0)
                {
                    System.out.println("[-] An error occurred with the payload size...");
                    break;
                }
                ByteBuffer bb = ByteBuffer.wrap(pSize).order(ByteOrder.LITTLE_ENDIAN);
                int size = bb.getInt();

                //Reading message payload
                byte[] payloadBytes = in.readNBytes(size);
                if(payloadBytes.length != size)
                {
                    System.out.println("[-] An error occurred with the payload...");
                    break;
                }

                Message message = Message.parseMessage(type, size, payloadBytes);
                if(message != null)
                {
                    System.out.println("New message receive");
                    if(message.getType() == MessageType.INIT){
                        /* CREATE THE AGENT ID */
                        Random rand = new Random();
                        int agentId = (int) (((System.currentTimeMillis() / 1000) + rand.nextInt(100000)) % 100000);
                        System.out.println(agentId);
                        ByteBuffer agentBb = ByteBuffer.allocate(4).order(ByteOrder.BIG_ENDIAN);
                        agentBb.putInt(agentId);
                        byte[] response = Message.createResponse(MessageType.DEBUG, agentBb.array());
                        if(response != null)
                            out.write(response);
                        else
                            out.close();
                    }
                }
            }
            catch (IOException e)
            {
                System.out.println(("An error occurred during server initialization..." + e.getMessage()));
                throw new IOException();
            }

        }

    }

}
