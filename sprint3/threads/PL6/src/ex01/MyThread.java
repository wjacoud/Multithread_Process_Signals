package ex01;

public class MyThread extends Thread {

    private OrderTaker taker;
    private OrderQueue queue;

    public MyThread(OrderTaker group, String name, OrderQueue queue) {
        super(group, name);
        this.taker = group;
        this.queue = queue;

    }

    public void run() {
        try{
            System.out.println("Thread "+ Thread.currentThread().getName() +" a correr!");
            Order order = new Order((int) Thread.currentThread().getId());
            taker.addOrderToQueue(queue, order);
            System.out.println("Add id: " + Thread.currentThread().getId());
        } catch (Exception ignored) {

        }
    }

}
