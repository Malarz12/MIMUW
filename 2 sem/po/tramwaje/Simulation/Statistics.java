package Simulation;

public class Statistics {
    private final int lengthOfSimulation; // Length of the simulation in days
    private int[] numberOfTrips;
    private int[] timeOfWaiting;
    private int[] numberOfHalts;
    private int numberOfAllTrips;
    private int wholeTimeOfWaiting;
    private int wholeNumberOfHalts;
    private double averageTimeOfWaiting;

    public Statistics(int lengthOfSimulation) {
        this.lengthOfSimulation = lengthOfSimulation;
        this.numberOfAllTrips = 0;
        this.averageTimeOfWaiting = 0;
        this.wholeTimeOfWaiting = 0;

        numberOfTrips = new int[lengthOfSimulation];
        timeOfWaiting = new int[lengthOfSimulation];
        numberOfHalts = new int[lengthOfSimulation];
    }
    public void printStatistics() {
        for(int i = 0; i < lengthOfSimulation; i++) {
            System.out.println("Dzien " + (i + 1) + " ilosc przejazdow: " + numberOfTrips[i] + ", czas czekania " + timeOfWaiting[i]
            + " ,liczba postojow na przystanku " + numberOfHalts[i]);

        }

        System.out.println("Liczba wszystkich przejazdow: " + numberOfAllTrips);
        System.out.println("Liczba wszystkich posiedzeń na przystanku: " + wholeNumberOfHalts);
        System.out.println("Laczny czas czekania na przystanku: " + wholeTimeOfWaiting);
        System.out.println("Sredni czas czekania: " + String.format("%.2f", ((double)wholeTimeOfWaiting / (double)wholeNumberOfHalts)));;
    }

    public int getLengthOfSimulation() {
        return lengthOfSimulation;
    }

    public int[] getTimeOfWaiting() {
        return timeOfWaiting;
    }

    public int[] getNumberOfTrips() {
        return numberOfTrips;
    }
    public int[] getNumberOfHalts() {
        return numberOfHalts;
    }

    public int getWholeTimeOfWaiting() {
        return wholeTimeOfWaiting;
    }

    public double getAverageTimeOfWaiting() {
        return averageTimeOfWaiting;
    }

    public int getWholeNumberOfHalts() {
        return wholeNumberOfHalts;
    }

    public int getNumberOfAllTrips() {
        return numberOfAllTrips;
    }

    public void increaseNumberOfHalts(int i) {
        numberOfHalts[i]++;
        wholeNumberOfHalts++;
    }
    public void increaseNumberOfTips(int i) {
        numberOfTrips[i]++;
        numberOfAllTrips++;
    }

    public void increaseTimeOfWaiting(int i,int n) {
        timeOfWaiting[i] = timeOfWaiting[i] + n;
        wholeTimeOfWaiting = wholeTimeOfWaiting + n;
    }
    public void calculateAverageTimeOfWaiting() {
        averageTimeOfWaiting = (double)wholeTimeOfWaiting / (double)wholeNumberOfHalts;
    }

}
