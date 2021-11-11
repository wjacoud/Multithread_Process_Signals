package ex01;

public class Main {


    public static void main(String[] args) {
        //Classe fila implementada para guardar os pedidos
        OrderQueue queue = new OrderQueue();

        //Arrays de controle das threads que adicionam e removem pedidos
        Thread[] takersThreads = new Thread[10];
        Thread[] handlersThreads = new Thread[10];

        //Cria as threads e os objs tipo order takers
        for (int i = 0; i < takersThreads.length; i++) {
            OrderTaker orderTaker = new OrderTaker(queue);
            takersThreads[i] = new MyThread(orderTaker, "Taker-" + i, queue);
            takersThreads[i].start();
        }

        //Espera o fim das threads orderTaker
        for (Thread tOrderTaker : takersThreads) {
            try {
                tOrderTaker.join();
            } catch (InterruptedException ignored) {
            }
        }

        System.out.println("All order takers ended...");
        System.out.println("Printing the queue ... ");

        //
        for (int i = 0; i < queue.getOrderQueue().size(); i++) {
            System.out.println("Queue n: " + queue.getOrderQueue().get(i).getId() + " ");
        }

        for (int i = 0; i < handlersThreads.length; i++) {
            OrderHandler orderHandler = new OrderHandler(queue);
            handlersThreads[i] = new OtherThread(orderHandler, "Handler-" + i, queue);
            handlersThreads[i].start();
        }

        for (Thread tOrderHanlder : handlersThreads) {
            try {
                tOrderHanlder.join();
            } catch (InterruptedException ignored) {
            }
        }

        System.out.println("All order handlers ended...");
        System.out.println("Size of the queue ... " + queue.getOrderQueue().size());


    }

}

