import Investors.Investor;
import Investors.RANDOM;
import Investors.SMA;
import Orders.Order;
import Simulation.*;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.HashMap;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class SMATest{

    private Simulation simulation;
    private ArrayList<Investor> investors;
    private ArrayList<String> shares;
    private ArrayList<Integer> sharesLastPrices;
    private final String A = "A";
    @Before
    public void setUp() {
       simulation = new Simulation();
       HashMap<String, Integer> w1 = new HashMap<>();
       w1.put(A, 10000);

       Investor i1 = new SMA(10000,w1,simulation);
       investors = new ArrayList<>();
       investors.add(i1);
       shares = new ArrayList<>();
       shares.add(A);
       sharesLastPrices = new ArrayList<>();
       sharesLastPrices.add(0);
       simulation.getData(investors,shares,sharesLastPrices, 1200);
    }
    @Test
    public void test1() {
        ShareSimulation shareSimulation = new ShareSimulation(sharesLastPrices.getFirst(), null);
        simulation.setEachShareSimulation(A, shareSimulation);
        ArrayList<Integer> lastPrices = new ArrayList<>();
        lastPrices.add(1);
        lastPrices.add(1);
        lastPrices.add(1);
        lastPrices.add(1);
        lastPrices.add(1);
        lastPrices.add(10);
        lastPrices.add(10);
        lastPrices.add(10);
        lastPrices.add(10);
        lastPrices.add(10);
        shareSimulation.setLastTransactionsPrices(lastPrices);
        Order order = investors.getFirst().getOrder(10,12);
        assertTrue(order.getType());
    }
    @Test
    public void test2() {
        ShareSimulation shareSimulation = new ShareSimulation(sharesLastPrices.getFirst(), null);
        simulation.setEachShareSimulation(A, shareSimulation);
        ArrayList<Integer> lastPrices = new ArrayList<>();
        lastPrices.add(5);
        lastPrices.add(5);
        lastPrices.add(5);
        lastPrices.add(5);
        lastPrices.add(5);
        lastPrices.add(6);
        lastPrices.add(5);
        lastPrices.add(5);
        lastPrices.add(5);
        lastPrices.add(5);
        lastPrices.add(5);
        shareSimulation.setLastTransactionsPrices(lastPrices);
        Order order = investors.getFirst().getOrder(10,12);
        assertFalse(order.getType());
    }
}

