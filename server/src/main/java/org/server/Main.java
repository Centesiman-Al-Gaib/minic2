package org.server;
import java.math.BigInteger;
import java.net.*;
import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class Main {
    public static void main(String[] args) {
    Server server = Server.CreateServer(8888);
        try {
            server.runServer();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}