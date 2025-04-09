package org.server.message;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public abstract class Message {

    MessageType type;
    Integer size;
    ByteBuffer payload;

    public Message(MessageType type, Integer size, ByteBuffer payload)
    {
        this.type = type;
        this.size = size;
        this.payload = payload;
    }

}
