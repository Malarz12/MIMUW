import Simulation.*;

import java.util.Scanner;

public class Main{
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        String filePath = scanner.next();
        int length = scanner.nextInt();
        Simulation simulation = new Simulation();
        ReadData data = new ReadData(filePath, simulation);
        simulation.getData(data.getInvestors(), data.getShares(), data.getLastPrices(), length);
        scanner.close();
        simulation.doSimulation();
        simulation.printResult();
        simulation.printNumberOfAllTransacion();
        simulation.printNumberOfTransactionsForEachShare();
        simulation.printNumberOfPurchasesForEachTypeOfOrder();
        simulation.printNumberOfSalesForEachTypeOfOrder();
    }
}