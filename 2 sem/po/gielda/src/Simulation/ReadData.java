package Simulation;

import Investors.Investor;
import Investors.RANDOM;
import Investors.SMA;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

// Reads and processes data from a file to initialize simulation parameters.
public class ReadData {
    private Simulation simulation;
    private int numberOfRandomInvestors;
    private int numberOfSMAInvestors;
    private ArrayList < String > shares; // List of share names read from the file.
    private ArrayList < Integer > lastPrices; // List of last prices of shares read from the file.
    private HashMap < String, Integer > stocksMap; // Map of share names to amounts held by the simulation.
    private int cash; // Initial cash amount for the simulation.

    // Constructor that initializes the ReadData object with data from a specified file.
    public ReadData(String filePath, Simulation simulation) {
        this.simulation = simulation;
        this.shares = new ArrayList < > ();
        this.lastPrices = new ArrayList < > ();
        this.stocksMap = new HashMap < > ();
        try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
            String line;
            int l = 0;
            while ((line = br.readLine()) != null) {
                if (!line.startsWith("#")) { // Ignore commented lines starting with #
                    if (l == 0) {
                        // Count the number of random (R) and SMA (S) investors.
                        for (int i = 0; i < line.length(); i++) {
                            char currentChar = line.charAt(i);
                            if (currentChar == 'R') {
                                numberOfRandomInvestors++;
                            } else if (currentChar == 'S') {
                                numberOfSMAInvestors++;
                            } else if (currentChar != ' ') {
                                throw new IllegalArgumentException("There is no such investor: " + currentChar);
                            }
                        }
                        l++;
                    } else if (l == 1) {
                        // Read share name and last price pairs.
                        String[] pairs = line.split("\\s+");
                        for (String pair: pairs) {
                            String[] parts = pair.split(":");
                            if (parts.length == 2) {
                                String shareName = parts[0];
                                int lastPrice = Integer.parseInt(parts[1]);
                                shares.add(shareName);
                                lastPrices.add(lastPrice);
                            } else {
                                System.out.println("Wrong pair " + pair);
                            }
                        }
                        l++;
                    } else if (l == 2) {
                        // Read initial cash and share amounts for the simulation.
                        String[] parts = line.split("\\s+");
                        this.cash = Integer.parseInt(parts[0]);
                        for (int i = 1; i < parts.length; i++) {
                            String pair = parts[i];
                            String[] pairParts = pair.split(":");
                            if (pairParts.length == 2) {
                                String shareName = pairParts[0];
                                int amount = Integer.parseInt(pairParts[1]);
                                if (shares.contains(shareName)) {
                                    stocksMap.put(shareName, amount);
                                } else {
                                    throw new IllegalArgumentException("Not correct Name: " + shareName);
                                }
                            } else {
                                throw new IllegalArgumentException("Wrong pair: " + pair);
                            }
                        }
                        l++;
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Returns the list of share names.
    public ArrayList < String > getShares() {
        return shares;
    }

    // Creates and returns a list of investors based on the read data.
    public ArrayList < Investor > getInvestors() {
        ArrayList < Investor > result = new ArrayList < > ();
        for (int i = 0; i < numberOfRandomInvestors; i++) {
            result.add(new RANDOM(cash, stocksMap, simulation));
        }
        for (int i = 0; i < numberOfSMAInvestors; i++) {
            result.add(new SMA(cash, stocksMap, simulation));
        }
        return result;
    }

    // Returns the list of last prices of shares.
    public ArrayList < Integer > getLastPrices() {
        return lastPrices;
    }

    // Returns the number of random investors.
    public int getNumberOfRandomInvestors() {
        return numberOfRandomInvestors;
    }

    // Returns the number of SMA investors.
    public int getNumberOfSMAInvestors() {
        return numberOfSMAInvestors;
    }
}