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
        String commandLine = "whoami";
        ByteBuffer bb = ByteBuffer.wrap(commandLine.getBytes()).order(ByteOrder.BIG_ENDIAN);
        return Message.createResponse(MessageType.COMMAND_LINE_EXECUTION,bb.array());
    }
}
