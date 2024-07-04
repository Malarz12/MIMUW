package Queue;

import Orders.Order;

import java.util.ArrayList;
import java.util.Comparator;

// Queue abstract class.
public abstract class Queue {
    private ArrayList < Order > queue; // ArrayList to store orders.

    // Constructor to initialize the queue.
    public Queue() {
        this.queue = new ArrayList < > (); // Initialize an empty ArrayList.
    }
    // Constructor to clone the queue.
    public Queue(Queue original) {
        this.queue = new ArrayList<>();
        for (Order order : original.getQueue()) {
            this.queue.add(order.clone()); // Add a clone of each order to the new queue.
        }
    }
    // Method to add an order to the queue.
    public void add(Order order) {
        queue.add(order); // Add an order to the queue.
    }

    // Method to get the first order in the queue.
    public Order getFirstOrder() {
        return queue.getFirst(); // Get the first order in the queue.
    }

    // Method to retrieve the entire queue.
    public ArrayList < Order > getQueue() {
        return queue; // Get the ArrayList containing all orders.
    }

    // Method to remove the first order from the queue.
    public void removeFirst() {
        queue.removeFirst(); // Remove the first order from the queue.
    }

    // Abstract method to compare orders based on price (to be implemented by subclasses).
    protected abstract boolean comparePrice(Order order1, Order order2);

    // Method to compare orders based on turn and then whichInTurn if turns are equal.
    protected boolean compareTurn(Order order1, Order order2) {
        if (order1.getTurn() > order2.getTurn()) {
            return true; // Compare orders based on turn.
        } else if (order1.getTurn() < order2.getTurn()) {
            return false; // Compare orders based on turn.
        } else {
            return compareWhichInTurn(order1, order2); // Compare based on whichInTurn if turns are equal.
        }
    }

    // Method to compare orders based on whichInTurn.
    protected boolean compareWhichInTurn(Order order1, Order order2) {
        return order2.getWhichInTurn() < order1.getWhichInTurn(); // Compare orders based on whichInTurn.
    }

    // Method to sort the queue using a comparator based on price and turn.
    public void sortQueue() {
        queue.sort(new Comparator < Order > () {
            @Override
            public int compare(Order o1, Order o2) {
                if (comparePrice(o1, o2)) {
                    return 1; // Sort orders in ascending order based on price.
                } else if (comparePrice(o2, o1)) {
                    return -1; // Sort orders in descending order based on price.
                } else {
                    return 0; // Orders have equal priority.
                }
            }
        });
    }

    // Method to retrieve an order at a specific index in the queue.
    public Order get(int i) {
        return queue.get(i); // Return the order at index i in the queue.
    }
}