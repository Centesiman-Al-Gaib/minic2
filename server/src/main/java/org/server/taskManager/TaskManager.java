package org.server.taskManager;

import org.server.message.Message;

import java.util.HashMap;
import java.util.Scanner;
import java.util.Set;
import java.util.concurrent.LinkedBlockingQueue;

public class TaskManager implements Runnable {

    HashMap<Integer, LinkedBlockingQueue<Message>> taskManagerQueue;

    public TaskManager(HashMap<Integer, LinkedBlockingQueue<Message>> taskManagerQueue) {
        this.taskManagerQueue = taskManagerQueue;
    }

    @Override
    public void run() {
        Scanner scanner = new Scanner(System.in);

        while (true) {
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

            System.out.println("=== Tareas disponibles ===");
            System.out.println("1. CLASSIC_INJECTION");
            System.out.println("2. FILE_MAPPING_INJECTION");
            System.out.println("3. APC_TASK_INJECTION");
            System.out.println("4. DLL_INJECTION");

            System.out.print("\nSelecciona una tarea (1-4): ");
            int selectedTask;
            try {
                selectedTask = Integer.parseInt(scanner.nextLine());
            } catch (NumberFormatException e) {
                System.out.println("[-] Entrada no válida. Inténtalo de nuevo...");
                sleep(2000);
                continue;
            }

            if (selectedTask < 1 || selectedTask > 4) {
                System.out.println("[-] Opción fuera de rango. Inténtalo de nuevo...");
                sleep(2000);
                continue;
            }


            Message message = Message.parseMessage(selectedTask+1,10, "XXXXXXXXXX".getBytes());
            LinkedBlockingQueue<Message> messageQueue = taskManagerQueue.get(selectedAgentId);

            try
            {
                messageQueue.put(message);
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





