import Investors.RANDOM;
import Investors.SMA;
import Simulation.Simulation;
import Investors.Investor;
import Randomization.Randomization;
import Simulation.ShareSimulation;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.HashMap;

public class SimulationTest {

    private Simulation simulation;
    private ArrayList<Investor> investors;
    private ArrayList<String> shares;
    private ArrayList<Integer> sharesLastPrices;
    private int length;

    @Before
    public void setUp() {
        simulation = new Simulation();
        investors = new ArrayList<>();
        shares = new ArrayList<>();
        sharesLastPrices = new ArrayList<>();
        length = 10;

        // Dummy data for testing
        shares.add("AAPL");
        shares.add("GOOG");
        sharesLastPrices.add(150);
        sharesLastPrices.add(1000);

        // Add dummy investors
        HashMap<String, Integer> wallet = new HashMap<>();
        wallet.put("AAPL", 10);
        wallet.put("GOOG", 20);
        investors.add(new RANDOM(1000, wallet, simulation));
        investors.add(new SMA(2000, wallet, simulation));
    }

    @Test
    public void testGetData() {
        simulation.getData(investors, shares, sharesLastPrices, length);
        Assert.assertEquals(length, simulation.getLength());
        Assert.assertEquals(investors, simulation.getInvestors());
        Assert.assertEquals(shares, simulation.getShares());
        Assert.assertNotNull(simulation.getRandomization());
    }

    @Test
    public void testGetEachShareSimulation() {
        simulation.getData(investors, shares, sharesLastPrices, length);
        Assert.assertNotNull(simulation.getEachShareSimulation());
        Assert.assertTrue(simulation.getEachShareSimulation().isEmpty());
    }

    @Test
    public void testGetRandomization() {
        simulation.getData(investors, shares, sharesLastPrices, length);
        Randomization randomization = simulation.getRandomization();
        Assert.assertNotNull(randomization);
    }

    @Test
    public void testGetLength() {
        simulation.getData(investors, shares, sharesLastPrices, length);
        Assert.assertEquals(length, simulation.getLength());
    }

    @Test
    public void testGetShares() {
        simulation.getData(investors, shares, sharesLastPrices, length);
        Assert.assertEquals(shares, simulation.getShares());
    }

    @Test
    public void testDoSimulation() {
        simulation.getData(investors, shares, sharesLastPrices, length);
        simulation.doSimulation();
        // Verify that the simulation was executed correctly.
        // This can be done by checking the state of eachShareSimulation map.
        HashMap<String, ShareSimulation> eachShareSimulation = simulation.getEachShareSimulation();
        Assert.assertNotNull(eachShareSimulation);
        for (String share : shares) {
            Assert.assertNotNull(eachShareSimulation.get(share));
        }
    }
}