package Queue;

import Orders.Order;

// SaleQueue class extending Queue for managing sale orders.
public class SaleQueue extends Queue {

    // Constructor initializing the queue.
    public SaleQueue() {
        super(); // Call superclass constructor to initialize the queue.
    }
    public SaleQueue(Queue queue) {
        super(queue);
    }
    // Method to compare sale orders based on price and turn.
    @Override
    protected boolean comparePrice(Order order1, Order order2) {
        if (order1.getPrice() < order2.getPrice()) {
            return false; // order1 should not come before order2 (ascending order by price).
        } else if (order1.getPrice() > order2.getPrice()) {
            return true; // order1 should come before order2 (descending order by price).
        } else {
            return compareTurn(order1, order2); // Compare based on turn if prices are equal.
        }
    }
}