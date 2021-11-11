package ex01;

public class OrderTaker  implements Runnable{


    private OrderQueue orderQueue;


    public OrderTaker(OrderQueue orderQueue) {
        this.orderQueue = orderQueue;
    }

    public OrderQueue createOrderQueue(){
        return new OrderQueue();
    }

    public void addOrderToQueue(OrderQueue queue, Order order) {
        synchronized (queue){
            queue.getOrderQueue().add(order);
        }

    }

    @Override
    public void run() {

    }
}
