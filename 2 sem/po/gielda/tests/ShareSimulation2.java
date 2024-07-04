import Investors.RANDOM;
import Orders.*;
import Investors.Investor;
import Investors.SMA;
import Simulation.ShareSimulation;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.HashMap;

import static org.junit.Assert.*;

public class ShareSimulation2 {
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
        wallet1.put("AAPL", 1000);
        investor1 = new SMA(10000, wallet1, null);

        HashMap<String, Integer> wallet2 = new HashMap<>();
        wallet2.put("AAPL", 5000);
        investor2 = new RANDOM(2000, wallet2, null);

        // Initializing orders using the new constructor.
        order1 = new OpenEndedOrder(1, 50, investor1, "AAPL", 1, 0, true); // Purchase order.
        order2 = new ImmediateOrder(1, 50, investor2, "AAPL", 1, 1, false); // Sale order.
    }

    @Test
    public void testGetLastTransactionPrice() {
        assertEquals(100, shareSimulation.getLastTransactionPrice());
    }

    @Test
    public void testGetTurn() {
        assertEquals(0, shareSimulation.getTurn());
    }

    @Test
    public void testTransaction() {
        boolean result = shareSimulation.transaction(order1, order2, true);

        assertTrue(result);
        assertEquals(9950, investor1.getCash());
        assertEquals(2050, investor2.getCash());
        assertEquals(50, shareSimulation.getLastTransactionPrice());
    }

    @Test
    public void testMultipleTransactions() {
        Order order3 = new OpenEndedOrder(2, 60, investor1, "AAPL", 1, 0, true); // Purchase order.
        Order order4 = new ExecuteOrCancelOrder(1, 70, investor2, "AAPL", 1, 1, false); // Sale order.

        shareSimulation.transaction(order1, order2, true); // Turn 0
        shareSimulation.nextTurn(); // Turn 1
        shareSimulation.transaction(order3, order4, true); // Turn 1

        assertEquals(9890, investor1.getCash());
        assertEquals(2110, investor2.getCash());
        assertEquals(60, shareSimulation.getLastTransactionPrice());
    }

    @Test
    public void testLargeVolumeTransaction() {
        Order order5 = new OpenEndedOrder(100, 50, investor1, "AAPL", 1, 0, true); // Purchase order.
        Order order6 = new TurnBasedOrder(80, 50, investor2, "AAPL", 1, 1, false,3); // Sale order.

        boolean result = shareSimulation.transaction(order5, order6, true);

        assertTrue(result);
        assertEquals(6000, investor1.getCash());
        assertEquals(6000, investor2.getCash());
        assertEquals(50, shareSimulation.getLastTransactionPrice());
    }

    @Test
    public void testAveragePriceCalculations() {
        for (int i = 0; i < 5; i++) {
            shareSimulation.nextTurn();
        }

        assertEquals(100.0, shareSimulation.getAveragePriceOfNLastTurns(5), 0.01);
        assertEquals(100.0, shareSimulation.getAveragePriceOfNLastTurnStartingFromK(3, 3), 0.01);
    }

    @Test
    public void testCleanupMethod() {
        shareSimulation.getSaleQueue().add(order2);

        shareSimulation.cleanup();

        assertEquals(5001, investor2.getWallet().get("AAPL").intValue());
        assertTrue(shareSimulation.getSaleQueue().getQueue().isEmpty());
    }
}