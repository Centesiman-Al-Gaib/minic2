package org.server.message;

public class InitMessage extends Message{

    public InitMessage(MessageType type, Integer size, byte[] payload) {
        super(type, size, payload);
    }
}
