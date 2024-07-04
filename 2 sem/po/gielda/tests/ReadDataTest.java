import Investors.Investor;
import Investors.RANDOM;
import Investors.SMA;
import Simulation.ReadData;
import Simulation.Simulation;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.HashMap;

import static org.junit.Assert.*;

public class ReadDataTest {
    private Simulation simulation;
    private ReadData readData;

    @Before
    public void setUp() {
        simulation = new Simulation();  // You might need to initialize Simulation based on your actual implementation.
        String filePath = "/Users/michalsmilowski/IdeaProjects/POprojekt2/res/data.txt";  // Adjust the file path accordingly for your test data.
        readData = new ReadData(filePath, simulation);
    }

    @Test
    public void testGetShares() {
        ArrayList<String> shares = readData.getShares();
        assertNotNull(shares);
        assertFalse(shares.isEmpty());
        assertEquals(2, shares.size());  // Assuming there are two shares in your test data.
        assertTrue(shares.contains("AAPL"));  // Replace with actual shares from your test data.
        assertTrue(shares.contains("GOOGL"));  // Replace with actual shares from your test data.
    }

    @Test
    public void testGetInvestors() {
        ArrayList<Investor> investors = readData.getInvestors();
        assertNotNull(investors);
        assertFalse(investors.isEmpty());
        assertEquals(readData.getNumberOfRandomInvestors() + readData.getNumberOfSMAInvestors(), investors.size());
        // Validate the types of investors created
        int randomCount = 0, smaCount = 0;
        for (Investor investor : investors) {
            if (investor instanceof RANDOM) {
                randomCount++;
            } else if (investor instanceof SMA) {
                smaCount++;
            }
        }
        assertEquals(readData.getNumberOfRandomInvestors(), randomCount);
        assertEquals(readData.getNumberOfSMAInvestors(), smaCount);
    }

    @Test
    public void testGetLastPrices() {
        ArrayList<Integer> lastPrices = readData.getLastPrices();
        assertNotNull(lastPrices);
        assertFalse(lastPrices.isEmpty());
        assertEquals(2, lastPrices.size());  // Assuming there are two shares in your test data.
        assertTrue(lastPrices.contains(100));  // Replace with actual last prices from your test data.
        assertTrue(lastPrices.contains(200));  // Replace with actual last prices from your test data.
    }
}