package Investors;

import Orders.Order;
import Simulation.Simulation;

import java.util.HashMap;

// RANDOM investor class, extending Investor.
public class RANDOM extends Investor {

    // Constructor for initializing RANDOM investor.
    public RANDOM(int cash, HashMap < String, Integer > wallet, Simulation simulation) {
        super(cash, wallet, simulation); // Calling superclass constructor.
    }

    @Override
    public Investor clone() {
        HashMap<String, Integer> wallet2 = new HashMap<>();
        wallet2.putAll(getWallet());
        RANDOM result = new RANDOM(getCash(), wallet2,getSimulation());
        return result;
    }

    // Override method to get an order for the RANDOM investor.
    @Override
    public Order getOrder(int turn, int whichInTurn) {
        // Using simulation's randomization to get a random order.
        return getSimulation().getRandomization().getRandomOrder(this, turn, whichInTurn);
    }
}