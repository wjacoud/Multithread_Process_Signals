package ex01;

import java.util.LinkedList;

public class OrderQueue {

    LinkedList<Order> orderQueue;

    public OrderQueue() {
        this.orderQueue = new LinkedList<Order>();
    }

    public LinkedList<Order> getOrderQueue() {
        return orderQueue;
    }

    public void setOrderQueue(LinkedList<Order> orderQueue) {
        this.orderQueue = orderQueue;
    }


}
