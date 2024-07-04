package Simulation;

import Investors.Investor;
import Orders.Order;
import Randomization.Randomization;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

// Class representing the simulation of trading among investors and shares.
public class Simulation {
    private ArrayList < Investor > investors; // List of investors participating in the simulation.
    private ArrayList < String > shares; // List of shares involved in the simulation.
    private HashMap < String, ShareSimulation > eachShareSimulation; // Map of share names to their respective simulations.
    private ArrayList < Integer > sharesLastPrices; // List of last known prices for each share.
    private int length; // Length of the simulation (number of turns).
    private Randomization randomization; // Randomization object for generating random orders.
    private int numberOfTransactions = 0; // Number of all transactions.

    private ArrayList<Integer> salesPerTypeOfOrders = new ArrayList<>(Collections.nCopies(4, 0));
    private ArrayList<Integer> purchasesPerTypeOfOrders = new ArrayList<>(Collections.nCopies(4, 0));
    // Empty constructor for Simulation.
    public Simulation() {}
    private ArrayList<Integer> numberOfTransactionsForEachShare = new ArrayList<>(); // As the name suggests.
    // Method to initialize simulation data.
    public void getData(ArrayList < Investor > investors, ArrayList < String > shares, ArrayList < Integer > sharesLastPrices, int length) {
        this.length = length; // Set the length of the simulation.
        this.investors = investors; // Set the list of investors.
        this.shares = shares; // Set the list of shares.
        this.sharesLastPrices = sharesLastPrices; // Set the list of last known prices for shares.
        this.randomization = new Randomization(this, shares); // Initialize randomization object.
        this.eachShareSimulation = new HashMap < > (); // Initialize map for each share simulation.
    }

    // Method to retrieve map of each share simulation.
    public HashMap < String, ShareSimulation > getEachShareSimulation() {
        return eachShareSimulation; // Return map of each share simulation.
    }
    // Method to retrieve randomization object.
    public Randomization getRandomization() {
        return randomization;
    }
    public ArrayList < Investor > getInvestors() {
        return investors;
    }
    // Method to retrieve length of the simulation.
    public int getLength() {
        return length;
    }

    // Method to retrieve list of shares involved in the simulation.
    public ArrayList < String > getShares() {
        return shares;
    }

    public void setEachShareSimulation(String share, ShareSimulation shareSimulation) {
        eachShareSimulation.put(share, shareSimulation);
    }
    // Method to execute the simulation.
    public void doSimulation() {
        // Initialize ShareSimulation objects for each share.
        for (int j = 0; j < shares.size(); j++) {
            eachShareSimulation.put(shares.get(j), new ShareSimulation(sharesLastPrices.get(j), this));
        }

        // Run the simulation for the specified number of turns.
        for (int i = 0; i < length; i++) {
            Collections.shuffle(investors); // Shuffle the list of investors for randomness.
            // Process each investor's order for the current turn.
            for (int j = 0; j < investors.size(); j++) {
                // Ensure investor has positive cash balance before placing an order.
                if (investors.get(j).getCash() > 0) {
                    Order order = investors.get(j).getOrder(i, j); // Get order from investor.
                    // Add order to appropriate queue based on order type (purchase or sale).
                   if (order != null && order.getType() == true) {
                        eachShareSimulation.get(order.getShare()).getPurchaseQueue().add(order);
                    } else if(order != null) {
                        eachShareSimulation.get(order.getShare()).getSaleQueue().add(order);
                    }
                }
            }

            // Process end of turn operations for each share simulation.
            for (String share: shares) {
                eachShareSimulation.get(share).endOfTurn();
                eachShareSimulation.get(share).nextTurn();
            }
        }

        // Clean up all remaining orders in each share simulation.
        for (int i = 0; i < shares.size(); i++) {
            increaseNumberOfTransaction(eachShareSimulation.get(shares.get(i)).getNumberOfTransactions());
            numberOfTransactionsForEachShare.add(eachShareSimulation.get(shares.get(i)).getNumberOfTransactions());
            for(int j = 0; j < 4; j++) {
                this.purchasesPerTypeOfOrders.set(j, this.purchasesPerTypeOfOrders.get(j)
                        + eachShareSimulation.get(shares.get(i)).getPurchasesPerTypeOfOrders().get(j));
                this.salesPerTypeOfOrders.set(j, this.salesPerTypeOfOrders.get(j)
                        + eachShareSimulation.get(shares.get(i)).getSalesPerTypeOfOrders().get(j));
            }
            eachShareSimulation.get(shares.get(i)).cleanup();
        }
    }
    // Output each investor's remaining cash and share holdings.
    public void printResult() {
        for (int i = 0; i < investors.size(); i++) {
            System.out.print(investors.get(i).getClass().getSimpleName() + " " + investors.get(i).getCash() + ": ");
            for (String share: shares) {
                System.out.print(share + ": " + investors.get(i).getWallet().get(share) + " ");
            }
            System.out.println();
        }
    }
    public void printNumberOfAllTransacion() {
        System.out.println("Number of All transactions: " + numberOfTransactions);
    }
    public void printNumberOfTransactionsForEachShare() {
        System.out.println();
        for(int i = 0; i < shares.size(); i++) {
            System.out.println("Number of transactions "+ shares.get(i) + ": " + numberOfTransactionsForEachShare.get(i));
        }
    }

    public void printNumberOfSalesForEachTypeOfOrder() {
        System.out.println();
        System.out.println("Sales: ");
        System.out.println("OpenEndedOrder: " + this.salesPerTypeOfOrders.get(0));
        System.out.println("TurnBaseOrder: " + this.salesPerTypeOfOrders.get(1));
        System.out.println("ImmidiateOrder: " + this.salesPerTypeOfOrders.get(2));
        System.out.println("ExecuteOrCancelOrder: " + this.salesPerTypeOfOrders.get(3));
    }

    public void printNumberOfPurchasesForEachTypeOfOrder() {
        System.out.println();
        System.out.println("Purchases: ");
        System.out.println("OpenEndedOrder: " + this.purchasesPerTypeOfOrders.get(0));
        System.out.println("TurnBaseOrder: " + this.purchasesPerTypeOfOrders.get(1));
        System.out.println("ImmidiateOrder: " + this.purchasesPerTypeOfOrders.get(2));
        System.out.println("ExecuteOrCancelOrder: " + this.purchasesPerTypeOfOrders.get(3));
    }
    private void increaseNumberOfTransaction(int n) {
        numberOfTransactions = numberOfTransactions + n;
    }
    public ArrayList<Investor> copyInvestors() {
        ArrayList<Investor> result = new ArrayList<>();
        for (int i = 0; i < investors.size(); i++) {
            result.add(investors.get(i).clone());
        }
        return result;
    }
}
