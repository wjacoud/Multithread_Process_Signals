package ex01;

public class OrderHandler implements Runnable {


    private OrderQueue orderQueue;

    public OrderHandler(OrderQueue orderQueue) {
        this.orderQueue = orderQueue;
    }

    public void removeOrderOfQueue(OrderQueue queue) {
        try{
            synchronized (queue) {
                while (!queue.getOrderQueue().isEmpty()){
                    queue.getOrderQueue().pop();
                }
            }
        } catch (Exception ignored) {
        }

    }

    @Override
    public void run() {

    }
}
