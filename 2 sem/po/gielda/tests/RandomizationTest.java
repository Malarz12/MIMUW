import Investors.SMA;
import Randomization.Randomization;
import Investors.Investor;
import Orders.*;
import Simulation.ShareSimulation;
import Simulation.Simulation;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import static org.junit.Assert.*;

public class RandomizationTest {
    private Randomization randomization;
    private ArrayList<String> shares;
    private Investor investor;

    @Before
    public void setUp() {
        shares = new ArrayList<>();
        shares.add("AAPL");
        shares.add("GOOGL");

        // Create a sample investor.
        HashMap<String, Integer> wallet = new HashMap<>();
        wallet.put("AAPL", 10);
        investor = new SMA(1000, wallet, null);

        Simulation simulation = new Simulation();
        simulation.getData(new ArrayList<>(), shares, new ArrayList<>(), 10); // Example initialization

        randomization = new Randomization(simulation, shares); // Pass null for Simulation since it's not used in these tests.
    }

    @Test
    public void testGetRandomShare() {
        String randomShare = randomization.getRandomShare();
        assertTrue(shares.contains(randomShare));
    }

    @Test
    public void testGetRandomPrice() {
        ShareSimulation shareSimulation = new ShareSimulation(100, null); // Assume last transaction price is 100.
        int randomPrice = randomization.getRandomPrice(shareSimulation);
        assertTrue(randomPrice > 0);
    }

    @Test
    public void testGetRandomNumberOfSharesToBuy() {
        int price = 50; // Assume price per share is 50.
        int numberOfShares = randomization.getRandomNumberOfSharesToBuy(investor, price);
        assertTrue(numberOfShares >= 0 && numberOfShares <= investor.getCash() / price);
    }

    @Test
    public void testGetRandomNumberOfSharesToSale() {
        String share = "AAPL";
        int numberOfShares = randomization.getRandomNumberOfSharesToSale(investor, share);
        assertTrue(numberOfShares > 0 && numberOfShares <= investor.getWallet().get(share));
    }
}

