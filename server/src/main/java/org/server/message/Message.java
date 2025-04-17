package org.server.message;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import static org.server.message.MessageType.*;

public abstract class Message {

    MessageType type;
    int size;
    byte[] payload;

    public static Message parseMessage(byte type, int pSize, byte[] payloadBytes)
    {
        MessageType eType;
        switch (type){
            case 0x1:
                eType = INIT;
                return new InitMessage(eType, pSize, payloadBytes);
            case 0xF:
                eType = DEBUG;
                return null;
            default:
                eType = UNKNOWN;
                return null;
        }
    }

    public static byte[] createResponse(MessageType messageType, byte[] payload)
    {
        byte type;
        switch (messageType) {
            case DEBUG:
                type = 0xF;
                break;
            default:
                return null;
        }
        int pSize = payload.length;
        if(pSize == 0)
        {
            return null;
        }
        ByteBuffer bb = ByteBuffer.allocate(4).order(ByteOrder.BIG_ENDIAN);
        bb.putInt(pSize);
        byte[] bSize = bb.array();

        bb = ByteBuffer.allocate(1 + 4 + pSize).order(ByteOrder.BIG_ENDIAN);

        bb.put(type);
        bb.put(bSize);
        bb.put(payload);

        return bb.array();
    }

    public Message()
    {

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

    public int getSize() {
        return size;
    }

    public byte[] getPayload() {
        return payload;
    }

}
