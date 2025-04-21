package org.server.message;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import static org.server.message.MessageType.*;

public class Message {

    MessageType type;
    int size;
    byte[] payload;

    public static Message parseMessage(int type, int pSize, byte[] payloadBytes)
    {
        MessageType eType = MessageType.values()[type];
        return new Message(eType, pSize, payloadBytes);
    }

    public static byte[] createResponse(MessageType messageType, byte[] payload)
    {
        int type = messageType.ordinal();
        int pSize = payload.length;
        if(pSize == 0)
        {
            return null;
        }
        ByteBuffer bb = ByteBuffer.allocate(4).order(ByteOrder.BIG_ENDIAN);
        bb.putInt(pSize);
        byte[] bSize = bb.array();

        bb = ByteBuffer.allocate(1 + 4 + pSize).order(ByteOrder.BIG_ENDIAN);

        bb.put((byte)type);
        bb.put(bSize);
        bb.put(payload);

        return bb.array();
    }

    public Message(MessageType type, Integer size, byte[] payload)
    {
        this.type = type;
        this.size = size;
        this.payload = payload;
    }

    public MessageType getType() {
        return type;
    }

    public byte[] getPayload() {
        return payload;
    }


}
