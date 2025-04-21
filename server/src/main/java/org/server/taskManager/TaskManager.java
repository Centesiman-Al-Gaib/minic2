package org.server.taskManager;

import org.server.handle.Handler;
import org.server.message.Message;
import org.server.message.MessageType;

import java.util.HashMap;
import java.util.Scanner;
import java.util.Set;
import java.util.concurrent.LinkedBlockingQueue;

public class TaskManager implements Runnable {

    HashMap<Integer, LinkedBlockingQueue<byte[]>> taskManagerQueue; // Links agents by ID with the messages that must be sended to them

    public TaskManager(HashMap<Integer, LinkedBlockingQueue<byte[]>> taskManagerQueue)
    {
        this.taskManagerQueue = taskManagerQueue;
    }

    @Override
    public void run()
    {
        Scanner scanner = new Scanner(System.in);

        while (true)
        {
            clearConsole();

            System.out.println("=== Agentes disponibles ===");
            Set<Integer> agentIds = taskManagerQueue.keySet();
            for (Integer id : agentIds) {
                System.out.println("Agente ID: " + id);
            }

            System.out.print("\nIntroduce el agentId deseado: ");
            int selectedAgentId;
            try {
                selectedAgentId = Integer.parseInt(scanner.nextLine());
            } catch (NumberFormatException e) {
                System.out.println("[-] Entrada no válida. Inténtalo de nuevo...");
                sleep(2000);
                continue;
            }

            if (!taskManagerQueue.containsKey(selectedAgentId)) {
                System.out.println("[-] AgentId no encontrado. Inténtalo de nuevo...");
                sleep(2000);
                continue;
            }

            clearConsole();
            int totalTasks = MessageType.values().length;
            System.out.println("\nSelecciona una tarea (1-"+totalTasks+"): ");
            for(int i = 0; i < totalTasks - 4; i ++)
            {
                System.out.println(MessageType.values()[i].ordinal() + ". " + MessageType.values()[i].toString());
            }

            int selectedTask;
            try {
                selectedTask = Integer.parseInt(scanner.nextLine());
            } catch (NumberFormatException e) {
                System.out.println("[-] Entrada no válida. Inténtalo de nuevo...");
                sleep(2000);
                continue;
            }

            if (selectedTask < 0 || selectedTask > totalTasks - 4) {
                System.out.println("[-] Opción fuera de rango. Inténtalo de nuevo...");
                sleep(2000);
                continue;
            }

            Message message = Message.parseMessage(selectedTask,10, "XXXXXXXXXX".getBytes());
            LinkedBlockingQueue<byte[]> messageQueue = taskManagerQueue.get(selectedAgentId);
            Handler handler = Handler.getHandle(message);
            byte[] messageToSend = handler.handle();
            try
            {
                messageQueue.put(messageToSend);
            }
            catch (InterruptedException e)
            {
                throw new RuntimeException(e);
            }
            System.out.println("[+] Mensaje preparado para el agente " + selectedAgentId);
            sleep(2000);
        }
    }

    private void clearConsole()
    {

    }

    private void sleep(long millis)
    {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}





