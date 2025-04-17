package org.server;

import org.server.handle.Handler;
import org.server.message.Message;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

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

                /* READING MESSAGE TYPE */
                byte type = (byte) in.read();

                /* READING PAYLOAD SIZE */
                byte[] pSize = in.readNBytes(MESSAGE_SIZE_FIELD_SIZE);
                if(pSize.length == 0)
                {
                    System.out.println("[-] An error occurred with the payload size...");
                    break;
                }
                ByteBuffer bb = ByteBuffer.wrap(pSize).order(ByteOrder.LITTLE_ENDIAN);
                int size = bb.getInt();

                /* READING PAYLOAD */
                byte[] payloadBytes = in.readNBytes(size);
                if(payloadBytes.length != size)
                {
                    System.out.println("[-] An error occurred with the payload...");
                    break;
                }

                /* GET MESSAGE OBJECT */
                Message message = Message.parseMessage(type, size, payloadBytes);
                if(message != null)
                {
                    /* HANDLE MESSAGE BY TYPE */
                    System.out.println("New message receive");
                    Handler handler = Handler.getHandle(message);
                    byte[] response = handler.handle();
                    if(response != null)
                    {
                        System.out.println("[+] Sending response to agent...");
                        out.write(response);
                    }

                    else
                    {
                        System.out.println("[-] An error occurred with the response creation...");
                        out.close();
                    }

                }
            }
            catch (IOException e)
            {
                System.out.println(("[-] An error occurred during server initialization..." + e.getMessage()));
                throw new IOException();
            }

        }

    }

}
