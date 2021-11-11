package ex01;

public class OtherThread extends Thread {

    private OrderHandler handler;
    private OrderQueue queue;

    public OtherThread(OrderHandler group, String name, OrderQueue queue) {
        super(group, name);
        this.handler = group;
        this.queue = queue;

    }

    public void run() {
        System.out.println("Thread "+ Thread.currentThread().getName() +" a correr!");
        handler.removeOrderOfQueue(queue);

    }
}

