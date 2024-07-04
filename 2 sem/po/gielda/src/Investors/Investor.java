package Investors;

import java.util.HashMap;
import Orders.Order;
import Simulation.Simulation;

// Abstract class representing an investor.
public abstract class Investor {
    private int cash; // Amount of cash the investor currently has.
    private HashMap < String, Integer > wallet; // HashMap storing stocks and their quantities in the investor's possession.
    private Simulation simulation; // Simulation instance associated with this investor.

    // Constructor for initializing the investor.
    public Investor(int cash, HashMap < String, Integer > wallet, Simulation simulation) {
        this.cash = cash;
        this.wallet = new HashMap < > ();
        this.wallet.putAll(wallet); // Initializing wallet with provided stocks.
        this.simulation = simulation; // Assigning simulation instance.
    }
    // Funcion used for cloning the Order.
    public abstract Investor clone();
    // Getter for retrieving the simulation instance associated with the investor.
    public Simulation getSimulation() {
        return simulation;
    }

    // Getter for retrieving the amount of cash the investor currently has.
    public int getCash() {
        return cash;
    }

    // Getter for retrieving the wallet containing stocks and their quantities.
    public HashMap < String, Integer > getWallet() {
        return wallet;
    }

    // Setter for updating the amount of cash the investor has.
    public void setCash(int cash) {
        this.cash = cash;
    }

    // Abstract method for getting an order, to be implemented by subclasses.
    public abstract Order getOrder(int turn, int whichInTurn);
}