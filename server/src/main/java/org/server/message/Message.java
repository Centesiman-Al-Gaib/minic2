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
        MessageType eType;
        switch (type)
        {
            case 0:
                eType = PING;
                break;
            case 1:
                eType = INIT;
                break;
            case 2:
                eType = CLASSIC_INJECTION;
                break;
            case 3:
                eType = FILE_MAPPING_INJECTION;
                break;
            case 4:
                eType = APC_TASK_INJECTION;
                break;
            case 5:
                eType = DLL_INJECTION;
                break;
            case 6:
                eType = COMMAND_LINE_EXECUTION;
                break;
            default:
                eType = UNKNOWN;
        }
        return new Message(eType, pSize, payloadBytes);

    }

    public static byte[] createResponse(MessageType messageType, byte[] payload)
    {
        int type;
        switch (messageType)
        {
            case PING:
                type = 0;
                break;
            case INIT:
                type = 1;
                break;
            case CLASSIC_INJECTION:
                type = 2;
                break;
            case FILE_MAPPING_INJECTION:
                type = 3;
                break;
            case APC_TASK_INJECTION:
                type = 4;
                break;
            case DLL_INJECTION:
                type = 5;
                break;
            case COMMAND_LINE_EXECUTION:
                type = 6;
                break;
            default:
                type = -1;
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

    public int getSize() {
        return size;
    }

    public byte[] getPayload() {
        return payload;
    }

}
