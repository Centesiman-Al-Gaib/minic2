package org.server.handle;

import org.server.message.Message;
import org.server.message.MessageType;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Random;

public class InitHandler implements Handler{
    @Override
    public byte[] handle()
    {
        /* CREATION OF RANDOM ID FOR AGENT */
        Random rand = new Random();
        int agentId = (int) (((System.currentTimeMillis() / 1000) + rand.nextInt(100000)) % 100000);

        /* SERLIALIZE INTEGER VALUE TO BYTES */
        ByteBuffer agentBb = ByteBuffer.allocate(4).order(ByteOrder.BIG_ENDIAN);
        agentBb.putInt(agentId);
        return Message.createResponse(MessageType.DEBUG, agentBb.array());
    }
}
