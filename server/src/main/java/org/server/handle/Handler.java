package org.server.handle;

import org.server.message.Message;

public interface Handler {

    byte[] handle();

    static Handler getHandle(Message message)
    {
        switch (message.getType())
        {
            case INIT:
                return new InitHandler();
            case PING:
                return new PingHandler();
        }
        return null;
    }

}
