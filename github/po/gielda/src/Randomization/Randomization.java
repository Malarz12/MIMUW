package Randomization;

import Investors.Investor;
import Orders.*;
import Simulation.ShareSimulation;
import Simulation.Simulation;

import java.util.ArrayList;
import java.util.Random;

// Randomization class for generating random orders and values.
public class Randomization {
    private Simulation simulation; // Reference to the simulation object.
    private ArrayList < String > shares; // List of available shares.
    private Random random; // Random object for generating random values.

    // Constructor to initialize the Randomization object.
    public Randomization(Simulation simulation, ArrayList < String > shares) {
        this.simulation = simulation; // Initialize simulation reference.
        this.shares = shares; // Initialize shares list.
        this.random = new Random(); // Initialize Random object.
    }

    // Method to get a random share from the list of shares.
    public String getRandomShare() {
        String randomShare = shares.get(random.nextInt(shares.size()));
        return randomShare; // Return the randomly selected share.
    }

    // Method to generate a random price for a share.
    public int getRandomPrice(ShareSimulation share) {
        int randomPrice = random.nextInt(20) + share.getLastTransactionPrice() - 10;
        if (randomPrice <= 0) {
            randomPrice = random.nextInt(share.getLastTransactionPrice() + 1) + 1;
        }
        return randomPrice; // Return the randomly generated price.
    }

    // Method to generate a random number of shares to buy based on investor's cash and price.
    public int getRandomNumberOfSharesToBuy(Investor investor, int price) {
        return random.nextInt(investor.getCash() / price); // Return the random number of shares to buy.
    }

    // Method to generate a random number of shares to sell based on investor's wallet.
    public int getRandomNumberOfSharesToSale(Investor investor, String share) {
        int randomNumberOfShares = random.nextInt(investor.getWallet().get(share)) + 1;
        return randomNumberOfShares; // Return the random number of shares to sell.
    }

    // Method to generate a random order (buy or sell).
    public Order getRandomOrder(Investor investor, int turn, int whichInTurn) {
        String share = getRandomShare(); // Get a random share.
        if (investor.getWallet().containsKey(share) && investor.getWallet().get(share) > 0) {
            return getRandomSale(investor, share, turn, whichInTurn); // Generate a random sale order.
        } else {
            return getRandomPurchase(investor, share, turn, whichInTurn); // Generate a random purchase order.
        }
    }

    // Method to generate a random sale order.
    public Order getRandomSale(Investor investor, String share, int turn, int whichInTurn) {
        int n = getRandomNumberOfSharesToSale(investor, share); // Random number of shares to sell.
        int p = getRandomPrice(simulation.getEachShareSimulation().get(share)); // Random price for the sale.
        int o = random.nextInt(4); // Random integer for selecting order type.
        investor.getWallet().put(share, investor.getWallet().get(share) - n); // Update investor's wallet after sale.
        if (o == 0) {
            return new ImmediateOrder(n, p, investor, share, turn, whichInTurn, false); // Return ImmediateOrder for sale.
        } else if (o == 1) {
            return new OpenEndedOrder(n, p, investor, share, turn, whichInTurn, false); // Return OpenEndedOrder for sale.
        } else if (o == 2) {
            return new TurnBasedOrder(n, p, investor, share, turn, whichInTurn, false, random.nextInt(simulation.getLength()) + turn); // Return TurnBasedOrder for sale.
        } else {
            return new ExecuteOrCancelOrder(n, p, investor, share, turn, whichInTurn, false); // Return ExecuteOrCancelOrder for sale.
        }
    }

    // Method to generate a random purchase order.
    public Order getRandomPurchase(Investor investor, String share1, int turn, int whichInTurn) {
        int p = getRandomPrice(simulation.getEachShareSimulation().get(share1)); // Random price for the purchase.
        if (p == 0) p++; // Ensure price is at least 1.
        if (investor.getCash() / p <= 0) {
            p = random.nextInt(investor.getCash()) + 1; // Adjust price if cash is insufficient for a single share.
        }
        int n = getRandomNumberOfSharesToBuy(investor, p); // Random number of shares to buy.
        int o = random.nextInt(4); // Random integer for selecting order type.
        if (o == 0) {
            return new ImmediateOrder(n, p, investor, share1, turn, whichInTurn, true); // Return ImmediateOrder for purchase.
        } else if (o == 1) {
            return new OpenEndedOrder(n, p, investor, share1, turn, whichInTurn, true); // Return OpenEndedOrder for purchase.
        } else if (o == 2) {
            return new TurnBasedOrder(n, p, investor, share1, turn, whichInTurn, true, random.nextInt(simulation.getLength() - turn) + turn); // Return TurnBasedOrder for purchase.
        } else {
            return new ExecuteOrCancelOrder(n, p, investor, share1, turn, whichInTurn, true); // Return ExecuteOrCancelOrder for purchase.
        }
    }
}