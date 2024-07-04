import Orders.OpenEndedOrder;
import Queue.PurchaseQueue;
import Queue.SaleQueue;
import Orders.Order;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class QueueSortingTest {

    private PurchaseQueue purchaseQueue;
    private SaleQueue saleQueue;

    @Before
    public void setUp() {
        purchaseQueue = new PurchaseQueue();
        saleQueue = new SaleQueue();
    }

    @Test
    public void testPurchaseQueueSorting() {
        // Adding orders to the purchase queue.
        Order order1 = new OpenEndedOrder(1, 100, null, "", 0, 0, true); // price: 100, turn: 0, whichInTurn: 0.
        Order order2 = new OpenEndedOrder(2, 150, null, "", 0, 0, true); // price: 150, turn: 0, whichInTurn: 0.
        Order order3 = new OpenEndedOrder(3, 120, null, "", 0, 0, true); // price: 120, turn: 0, whichInTurn: 0.
        Order order4 = new OpenEndedOrder(4, 120, null, "", 1, 0, true); // price: 120, turn: 1, whichInTurn: 0.
        Order order5 = new OpenEndedOrder(5, 120, null, "", 0, 1, true); // price: 120, turn: 0, whichInTurn: 1.

        purchaseQueue.add(order1);
        purchaseQueue.add(order2);
        purchaseQueue.add(order3);
        purchaseQueue.add(order4);
        purchaseQueue.add(order5);

        // Sorting the purchase queue.
        purchaseQueue.sortQueue();

        // Checking the order after sorting.
        assertEquals(order2, purchaseQueue.get(0)); // order3 (price 120, turn 0, whichInTurn 0) should be first.
        assertEquals(order3, purchaseQueue.get(1)); // order5 (price 120, turn 0, whichInTurn 1) should be second.
        assertEquals(order5, purchaseQueue.get(2)); // order4 (price 120, turn 1, whichInTurn 0) should be third.
        assertEquals(order4, purchaseQueue.get(3)); // order1 (price 100, turn 0, whichInTurn 0) should be fourth.
        assertEquals(order1, purchaseQueue.get(4)); // order2 (price 150, turn 0, whichInTurn 0) should be fifth.
    }

    @Test
    public void testSaleQueueSorting() {
        // Adding orders to the sale queue.
        Order order1 = new OpenEndedOrder(1, 100, null, "", 0, 0, false); // price: 100, turn: 0, whichInTurn: 0.
        Order order2 = new OpenEndedOrder(2, 150, null, "", 0, 0, false); // price: 150, turn: 0, whichInTurn: 0.
        Order order3 = new OpenEndedOrder(3, 120, null, "", 0, 0, false); // price: 120, turn: 0, whichInTurn: 0.
        Order order4 = new OpenEndedOrder(4, 120, null, "", 1, 0, false); // price: 120, turn: 1, whichInTurn: 0.
        Order order5 = new OpenEndedOrder(5, 120, null, "", 0, 1, false); // price: 120, turn: 0, whichInTurn: 1.

        saleQueue.add(order1);
        saleQueue.add(order2);
        saleQueue.add(order3);
        saleQueue.add(order4);
        saleQueue.add(order5);

        // Sorting the sale queue.
        saleQueue.sortQueue();

        // Checking the order after sorting.
        assertEquals(order1, saleQueue.get(0));
        assertEquals(order3, saleQueue.get(1));
        assertEquals(order5, saleQueue.get(2));
        assertEquals(order4, saleQueue.get(3));
        assertEquals(order2, saleQueue.get(4));
    }
}
