package Simulation;

import Orders.ExecuteOrCancelOrder;
import Orders.*;
import Queue.*;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

// Class representing the simulation of shares and transactions.
public class ShareSimulation {
    private int turn; // Current turn in the simulation.
    private Queue saleQueue; // Queue for sale orders.
    private Queue purchaseQueue; // Queue for purchase orders.
    private int lastTransactionPrice; // Price of the last transaction.
    private ArrayList < Integer > lastTransactionsPrices; // List of prices of last transactions.
    private int numberOfTransactions = 0; // Stores number of all transactions of this share.
    private Simulation simulation;
    private ArrayList<Integer> salesPerTypeOfOrders;
    private ArrayList<Integer> purchasesPerTypeOfOrders;
    // Constructor to initialize ShareSimulation with the last transaction price.
    public ShareSimulation(int lastTransactionPrice, Simulation simulation) {
        this.turn = 0; // Initialize turn to 0.
        this.saleQueue = new SaleQueue(); // Initialize sale queue.
        this.purchaseQueue = new PurchaseQueue(); // Initialize purchase queue.
        this.lastTransactionPrice = lastTransactionPrice; // Set initial last transaction price.
        this.lastTransactionsPrices = new ArrayList < > (); // Initialize list for last transaction prices.
        this.simulation = simulation;
        this.purchasesPerTypeOfOrders = new ArrayList<>(Collections.nCopies(4, 0));
        this.salesPerTypeOfOrders = new ArrayList<>(Collections.nCopies(4, 0));
    }

    // Method to get the last transaction price.
    public int getLastTransactionPrice() {
        return lastTransactionPrice;
    }

    // Method to get the list of last transaction prices.
    public ArrayList < Integer > getLastTransactionsPrices() {
        return lastTransactionsPrices;
    }

    // Method to get the current turn.
    public int getTurn() {
        return turn;
    }

    // Method to get the sale queue.
    public Queue getSaleQueue() {
        return saleQueue;
    }

    // Method to get the purchase queue.
    public Queue getPurchaseQueue() {
        return purchaseQueue;
    }

    // Method to advance to the next turn.
    public void nextTurn() {
        turn++;
        lastTransactionsPrices.add(lastTransactionPrice); // Add the last transaction price to the list.

        // Maintain a list of last 10 transaction prices.
        if (lastTransactionsPrices.size() > 11) {
            lastTransactionsPrices.remove(0); // Remove the oldest price if list exceeds 10 entries.
        }
    }

    // Method to execute a transaction between two orders.
    public boolean transaction(Order order1, Order order2, boolean real) {
        int numberOfShares = order1.getNumberOfShares();

        // Adjust number of shares if order2 has fewer shares.
        if (order2.getNumberOfShares() < numberOfShares) {
            numberOfShares = order2.getNumberOfShares();
        }

        int price = order2.getPrice();

        // Determine transaction price based on turn and whichInTurn values of orders.
        if (order1.getWhichInTurn() < order2.getWhichInTurn() ||
                (order2.getWhichInTurn() == order1.getWhichInTurn() && order1.getTurn() < order2.getTurn())) {
            price = order1.getPrice();
        }

        // Check if investor 1 has enough cash for the transaction.
        if (order1.getInvestor().getCash() >= order1.getPrice() * numberOfShares) {
            order1.getInvestor().setCash(order1.getInvestor().getCash() - price * numberOfShares); // Deduct cash from investor 1.
            order2.getInvestor().setCash(order2.getInvestor().getCash() + price * numberOfShares); // Add cash to investor 2.

            // Update investor 1's share holdings.
            order1.getInvestor().getWallet().put(order1.getShare(),
                    order1.getInvestor().getWallet().get(order1.getShare()) + numberOfShares);

            order1.setNumberOfShares(order1.getNumberOfShares() - numberOfShares); // Update remaining shares in order 1.
            order2.setNumberOfShares(order2.getNumberOfShares() - numberOfShares); // Update remaining shares in order 2.

            if(real) {
                lastTransactionPrice = price; // Update last transaction price.
                numberOfTransactions++;
            }
            return true; // Transaction successful.
        } else {
            return false; // Transaction failed due to insufficient funds.
        }
    }
    public void updateTransactionsPerTypeOfOrders(Order order) {
        if(order.getType()) {
            if(order.getClass() == OpenEndedOrder.class) {
                purchasesPerTypeOfOrders.set(0, purchasesPerTypeOfOrders.get(0) + 1);
            } else if(order.getClass() == TurnBasedOrder.class) {
                purchasesPerTypeOfOrders.set(1, purchasesPerTypeOfOrders.get(1) + 1);
            } else if(order.getClass() == ImmediateOrder.class) {
                purchasesPerTypeOfOrders.set(2, purchasesPerTypeOfOrders.get(2) + 1);
            } else {
                purchasesPerTypeOfOrders.set(3, purchasesPerTypeOfOrders.get(3) + 1);
            }
        } else {
            if(order.getClass() == OpenEndedOrder.class) {
                salesPerTypeOfOrders.set(0, salesPerTypeOfOrders.get(0) + 1);
            } else if(order.getClass() == TurnBasedOrder.class) {
                salesPerTypeOfOrders.set(1, salesPerTypeOfOrders.get(1) + 1);
            } else if(order.getClass() == ImmediateOrder.class) {
                salesPerTypeOfOrders.set(2, salesPerTypeOfOrders.get(2) + 1);
            } else {
                salesPerTypeOfOrders.set(3, salesPerTypeOfOrders.get(3) + 1);
            }
        }
    }
    // Method to process end of turn operations.
    public void endOfTurn() {
        Queue newSaleQueue = new SaleQueue(); // Create a new sale queue.
        Queue newPurchaseQueue = new PurchaseQueue(); // Create a new purchase queue.

        // Iterate through sale queue and remove orders that are no longer possible.
        for (int i = 0; i < saleQueue.getQueue().size(); i++) {
            if (!saleQueue.getQueue().get(i).isPossible(this)) {
                saleQueue.getQueue().remove(i);
                i--;
            }
        }

        // Iterate through purchase queue and remove orders that are no longer possible.
        for (int i = 0; i < purchaseQueue.getQueue().size(); i++) {
            if (!purchaseQueue.getQueue().get(i).isPossible(this)) {
                purchaseQueue.getQueue().remove(i);
                i--;
            }
        }

        saleQueue.sortQueue(); // Sort the sale queue.
        purchaseQueue.sortQueue(); // Sort the purchase queue.

        int i = 0;
        int j = 0;

        // Process transactions between sale and purchase queues.
        while (i < saleQueue.getQueue().size() && j < purchaseQueue.getQueue().size()) {
            if (purchaseQueue.get(j).getPrice() >= saleQueue.get(i).getPrice()) {
                boolean t = true;
                if (purchaseQueue.get(j).getClass() == ExecuteOrCancelOrder.class && saleQueue.get(i).getClass() == ExecuteOrCancelOrder.class) {
                    if (purchaseQueue.get(j).isPossibleWA(this, j, i, true, saleQueue, purchaseQueue)) {
                        if (saleQueue.get(i).isPossibleWA(this, j, i, false, saleQueue, purchaseQueue)) {
                            t = transaction(purchaseQueue.get(j), saleQueue.get(i), true);
                        } else t = false;
                    } else t = false;
                } else if (purchaseQueue.get(j).getClass() == ExecuteOrCancelOrder.class ) {
                    if (purchaseQueue.get(j).isPossibleWA(this, j, i, true, saleQueue, purchaseQueue)) {
                        t = transaction(purchaseQueue.get(j), saleQueue.get(i), true);
                    } else t = false;
                } else if (saleQueue.get(i).getClass() == ExecuteOrCancelOrder.class) {
                    if (saleQueue.get(i).isPossibleWA(this, j, i, false, saleQueue, purchaseQueue)) {
                        t = transaction(purchaseQueue.get(j), saleQueue.get(i), true);
                    } else t = false;
                } else {
                    t = transaction(purchaseQueue.get(j), saleQueue.get(i), true); // Execute transaction.
                }
                boolean b1 = false, b2 = false;

                if(t == true) {
                    updateTransactionsPerTypeOfOrders(saleQueue.get(i));
                    updateTransactionsPerTypeOfOrders(purchaseQueue.get(j));
                }
                // Add completed sale order to new sale queue if it's empty.
                if (saleQueue.get(i).getNumberOfShares() <= 0) {
                    newSaleQueue.add(saleQueue.get(i));
                    i++;
                    b1 = true;
                }

                // Add completed purchase order to new purchase queue if it's empty.
                if (purchaseQueue.get(j).getNumberOfShares() <= 0) {
                    newPurchaseQueue.add(purchaseQueue.get(j));
                    j++;
                    b2 = true;
                }

                if (!t) {
                    // If transaction failed, add orders back to respective queues.
                    if (!b1) {
                        newSaleQueue.add(saleQueue.get(i));
                        i++;
                    }
                    if (!b2) {
                        newPurchaseQueue.add(purchaseQueue.get(j));
                        j++;
                    }
                }
            } else {
                // Add orders to new queues if transaction condition is not met.
                newSaleQueue.add(saleQueue.get(i));
                newPurchaseQueue.add(purchaseQueue.get(j));
                i++;
                j++;
            }
        }

        // Add remaining orders from sale queue to new sale queue.
        for (int k = i; k < saleQueue.getQueue().size(); k++) {
            newSaleQueue.add(saleQueue.get(k));
        }

        // Add remaining orders from purchase queue to new purchase queue.
        for (int k = j; k < purchaseQueue.getQueue().size(); k++) {
            newPurchaseQueue.add(purchaseQueue.get(k));
        }

        saleQueue = newSaleQueue; // Update sale queue with new sale queue.
        purchaseQueue = newPurchaseQueue; // Update purchase queue with new purchase queue.
    }

    // Method to calculate the average price of the last N turns.
    public float getAveragePriceOfNLastTurns(int n) {
        float sum = 0;
        for (int i = 0; i < n; i++) {
            sum += lastTransactionsPrices.get(lastTransactionsPrices.size() - 1 - i);
        }
        return sum / n; // Return the average price.
    }

    // Method to calculate the average price of N turns starting from turn K.
    public float getAveragePriceOfNLastTurnStartingFromK(int n, int k) {
        // If there are fewer than 10 transactions, use getAveragePriceOfNLastTurns.
        if (lastTransactionsPrices.size() == 10) {
            return getAveragePriceOfNLastTurns(n);
        }
        float sum = 0;
        for (int i = 0; i < n; i++) {
            sum += lastTransactionsPrices.get(k - i);
        }
        return sum / n; // Return the average price.
    }

    // Method to clean up at the end of simulation.
    public void cleanup() {
        // Return all remaining shares in the sale queue back to investor's wallet.
        while (!saleQueue.getQueue().isEmpty()) {
            saleQueue.getFirstOrder().getInvestor().getWallet().put(saleQueue.getFirstOrder().getShare(),
                    saleQueue.getFirstOrder().getInvestor().getWallet().get(saleQueue.getFirstOrder().getShare()) +
                            saleQueue.getFirstOrder().getNumberOfShares());
            saleQueue.removeFirst(); // Remove the order from sale queue.
        }
    }

    public void setLastPrice(int lastTransactionPrice) {
        this.lastTransactionPrice = lastTransactionPrice;
    }
    public void setLastTransactionsPrices(ArrayList < Integer > lastTransactionsPrices) {
        this.lastTransactionsPrices = lastTransactionsPrices;
    }
    public void setTurn(int turn) {
        this.turn = turn;
    }

    public int getNumberOfTransactions() {
        return numberOfTransactions;
    }

    public Simulation getSimulation() {
        return  simulation;
    }

    public ArrayList<Integer> getPurchasesPerTypeOfOrders() {
        return purchasesPerTypeOfOrders;
    }

    public ArrayList<Integer> getSalesPerTypeOfOrders() {
        return salesPerTypeOfOrders;
    }
}