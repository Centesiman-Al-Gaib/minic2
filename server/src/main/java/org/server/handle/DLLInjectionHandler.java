package org.server.handle;

import org.server.message.Message;
import org.server.message.MessageType;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Random;

public class DLLInjectionHandler implements Handler{
    @Override
    public byte[] handle() {
        Random rand = new Random();
        int agentId = (int)(((System.currentTimeMillis() / 1000) + rand.nextInt(100000)) % 100000);

        /* SERIALIZE INTEGER VALUE TO BYTES */
        ByteBuffer agentBb = ByteBuffer.allocate(4).order(ByteOrder.BIG_ENDIAN);
        agentBb.putInt(agentId);
        return Message.createResponse(MessageType.DLL_INJECTION, agentBb.array());
    }
}
