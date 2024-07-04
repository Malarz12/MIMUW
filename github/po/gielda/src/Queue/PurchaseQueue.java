package Queue;

import Orders.Order;

// Represents a queue for purchasing orders.
public class PurchaseQueue extends Queue {

    // Default constructor.
    public PurchaseQueue() {
        super();
    }
    public PurchaseQueue(Queue queue) {
        super(queue);
    }
    // Compares two orders based on their prices.
    // Returns true if order1's price is less than or equal to order2's price,
    // otherwise compares based on turn.
    @Override
    protected boolean comparePrice(Order order1, Order order2) {
        if (order1.getPrice() > order2.getPrice()) {
            return false;  // order1 price is higher than order2 price.
        } else if (order1.getPrice() < order2.getPrice()) {
            return true;   // order1 price is lower than order2 price.
        } else
            return compareTurn(order1, order2);  // prices are equal, compare based on turn.
    }
}
