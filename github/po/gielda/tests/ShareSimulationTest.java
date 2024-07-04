import Simulation.ShareSimulation;
import Orders.Order;
import Orders.OpenEndedOrder; // Importing the new class.
import Investors.*;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.HashMap;

import static org.junit.Assert.*;

public class ShareSimulationTest {
    private ShareSimulation shareSimulation;
    private Investor investor1;
    private Investor investor2;
    private Order order1;
    private Order order2;

    @Before
    public void setUp() {
        shareSimulation = new ShareSimulation(100, null); // Initial transaction price is 100.

        // Initializing investors.
        HashMap<String, Integer> wallet1 = new HashMap<>();
        wallet1.put("AAPL", 10);
        investor1 = new SMA(1000, wallet1, null);

        HashMap<String, Integer> wallet2 = new HashMap<>();
        wallet2.put("AAPL", 5);
        investor2 = new SMA(2000, wallet2, null);

        // Initializing orders using the new constructor.
        order1 = new OpenEndedOrder(1, 50, investor1, "AAPL", 0, 0, true); // Purchase order.
        order2 = new OpenEndedOrder(1, 50, investor2, "AAPL", 0, 0, false); // Sale order.
    }

    @Test
    public void testGetLastTransactionPrice() {
        // Testing if the initial last transaction price is correctly set.
        assertEquals(100, shareSimulation.getLastTransactionPrice());
    }

    @Test
    public void testGetLastTransactionsPrices() {
        // Testing if the list of last transaction prices is initialized correctly.
        assertNotNull(shareSimulation.getLastTransactionsPrices());
        assertTrue(shareSimulation.getLastTransactionsPrices().isEmpty());
    }

    @Test
    public void testGetTurn() {
        // Testing if the initial turn is correctly set to 0.
        assertEquals(0, shareSimulation.getTurn());
    }

    @Test
    public void testGetSaleQueue() {
        // Testing if the sale queue is initialized correctly.
        assertNotNull(shareSimulation.getSaleQueue());
        assertTrue(shareSimulation.getSaleQueue().getQueue().isEmpty());
    }

    @Test
    public void testGetPurchaseQueue() {
        // Testing if the purchase queue is initialized correctly.
        assertNotNull(shareSimulation.getPurchaseQueue());
        assertTrue(shareSimulation.getPurchaseQueue().getQueue().isEmpty());
    }

    @Test
    public void testNextTurn() {
        // Testing if the next turn advances the simulation correctly.
        shareSimulation.nextTurn();
        assertEquals(1, shareSimulation.getTurn());
        assertEquals(1, shareSimulation.getLastTransactionsPrices().size());
        assertEquals(100, shareSimulation.getLastTransactionsPrices().get(0).intValue());
    }

    @Test
    public void testTransaction() {
        // Testing if a transaction between two orders is processed correctly.
        boolean result = shareSimulation.transaction(order1, order2, true);
        assertTrue(result);
        assertEquals(950, investor1.getCash());
        assertEquals(2050, investor2.getCash());
        assertEquals(50, shareSimulation.getLastTransactionPrice());
    }

    @Test
    public void testEndOfTurn() {
        // Testing if the end of turn processes the sale and purchase queues correctly.
        shareSimulation.getSaleQueue().add(order2);
        shareSimulation.getPurchaseQueue().add(order1);
        shareSimulation.endOfTurn();

        assertFalse(shareSimulation.getSaleQueue().getQueue().isEmpty());
        assertFalse(shareSimulation.getPurchaseQueue().getQueue().isEmpty());
    }

    @Test
    public void testGetAveragePriceOfNLastTurns() {
        // Testing if the average price of the last N turns is calculated correctly.
        for (int i = 0; i < 10; i++) {
            shareSimulation.nextTurn();
        }
        assertEquals(100.0, shareSimulation.getAveragePriceOfNLastTurns(10), 0.01);
    }

    @Test
    public void testGetAveragePriceOfNLastTurnStartingFromK() {
        // Testing if the average price of N turns starting from turn K is calculated correctly.
        for (int i = 0; i < 10; i++) {
            shareSimulation.nextTurn();
        }
        assertEquals(100.0, shareSimulation.getAveragePriceOfNLastTurnStartingFromK(10, 9), 0.01);
    }

    @Test
    public void testCleanup() {
        // Testing if the cleanup method processes remaining orders correctly.
        shareSimulation.getSaleQueue().add(order2);
        shareSimulation.cleanup();
        assertEquals(6, investor2.getWallet().get("AAPL").intValue());
        assertTrue(shareSimulation.getSaleQueue().getQueue().isEmpty());
    }
}