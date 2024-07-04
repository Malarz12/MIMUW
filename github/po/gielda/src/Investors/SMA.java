package Investors;

import Orders.Order;
import Simulation.Simulation;

import java.util.HashMap;
import java.util.Random;

// SMA investor class, extending Investor.
public class SMA extends Investor {

    // Constructor for initializing SMA investor.
    public SMA(int cash, HashMap < String, Integer > wallet, Simulation simulation) {
        super(cash, wallet, simulation); // Calling superclass constructor.
    }

    // Override method to get an order for the SMA investor.
    @Override
    public Order getOrder(int turn, int whichInTurn) {
        float sma5, sma10; // Variables for SMA values.
        float lastSma5, lastSma10; // Variables for previous SMA values.

        // If less than 10 turns have passed, make a random order.
        if (turn < 10) {
            return getSimulation().getRandomization().getRandomOrder(this, turn, whichInTurn);
        }
        for (int i = 0; i < getSimulation().getShares().size(); i++) {
            // Calculate SMA values for 5 and 10 turns
            sma5 = getSimulation().getEachShareSimulation().get(getSimulation().getShares().get(i)).getAveragePriceOfNLastTurns(5);
            sma10 = getSimulation().getEachShareSimulation().get(getSimulation().getShares().get(i)).getAveragePriceOfNLastTurns(10);

            // Calculate previous SMA values starting to check if line was crossed.
            lastSma5 = getSimulation().getEachShareSimulation().get(getSimulation().getShares().get(i)).getAveragePriceOfNLastTurnStartingFromK(5, 10);
            lastSma10 = getSimulation().getEachShareSimulation().get(getSimulation().getShares().get(i)).getAveragePriceOfNLastTurnStartingFromK(10, 10);

            if (((turn == 10 && sma5 <= sma10) || (turn > 10 && lastSma5 > lastSma10 && sma5 <= sma10)) &&
                    (getWallet().containsKey(getSimulation().getShares().get(i)) && getWallet().get(getSimulation().getShares().get(i)) > 0)) {
                return getSimulation().getRandomization().getRandomSale(this, getSimulation().getShares().get(i), turn, whichInTurn);
            }
            // Decide on buying conditions based on SMA.
            else if ((turn == 10 && sma5 >= sma10) || (turn > 10 && lastSma5 < lastSma10 && sma5 >= sma10)) {
                return getSimulation().getRandomization().getRandomPurchase(this, getSimulation().getShares().get(i), turn, whichInTurn);
            }
        }

        // Return a random order if no specific conditions were met.
        return null;
    }

    @Override
    public Investor clone() {
        HashMap<String, Integer> wallet2 = new HashMap<>();
        wallet2.putAll(getWallet());
        SMA result = new SMA(getCash(), wallet2,getSimulation());
        return result;
    }
}