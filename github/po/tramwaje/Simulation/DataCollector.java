package Simulation;
import Line.Line;
import ZTM.Stop;
import ZTM.Tram;

import java.util.Objects;
import java.util.Scanner;

public class DataCollector {
    private Line[] lines;
    private Stop[] stops;
    private int numberOfPassengers;
    private int lengthOfSimulation;

    public DataCollector() {
        Scanner scanner = new Scanner(System.in);

        // Read simulation parameters from user input
        lengthOfSimulation = scanner.nextInt(); // Length of the simulation (in days)
        int stopCapacity = scanner.nextInt(); // Capacity of each stop
        int numberOfStops = scanner.nextInt(); // Number of stops in the system

        // Create an array to store Stop objects
        stops = new Stop[numberOfStops];

        // Read stop names and initialize Stop objects
        for (int i = 0; i < numberOfStops; i++) {
            String stopName = scanner.next(); // Read stop name
            stops[i] = new Stop(stopCapacity, stopName); // Initialize Stop object with capacity and name
        }

        // Read parameters for passengers and trams
        numberOfPassengers = scanner.nextInt(); // Number of passengers in the simulation
        int tramCapacity = scanner.nextInt(); // Capacity of each tram
        int numberOfLines = scanner.nextInt(); // Number of tram lines

        // Create an array to store Line objects
        lines = new Line[numberOfLines];

        // Read parameters for each tram line and initialize Line objects
        for (int i = 0; i < numberOfLines; i++) {
            int numberOfTrams = scanner.nextInt(); // Number of trams on the line
            int lengthOfRoute = scanner.nextInt(); // Length of the route

            Stop[] routeStops = new Stop[lengthOfRoute]; // Array to store stops on the route
            int[] timeTable = new int[lengthOfRoute]; // Array to store time table information

            // Read stop names and times for each stop on the route
            for (int j = 0; j < lengthOfRoute; j++) {
                String stopName = scanner.next(); // Read stop name
                int time = scanner.nextInt(); // Read time
                // Search for the corresponding Stop object and initialize the routeStops array
                for (int k = 0; k < numberOfStops; k++) {
                    if (Objects.equals(stops[k].getName(), stopName)) {
                        routeStops[j] = stops[k]; // Initialize stop in the routeStops array
                        break;
                    }
                }
                timeTable[j] = time; // Initialize time in the timeTable array
            }

            // Create an array to store Tram objects
            Tram[] trams = new Tram[numberOfTrams];

            // Initialize Tram objects and set the line for each tram
            for (int j = 0; j < numberOfTrams; j++) {
                trams[j] = new Tram(tramCapacity, j); // Initialize Tram object with capacity and number
            }

            // Initialize Line object with stops, time table, and trams
            Line line = new Line(routeStops, timeTable, trams, i);
            lines[i] = line; // Add Line object to the lines array

            // Set the line for each tram on the line
            for (int j = 0; j < numberOfTrams; j++) {
                trams[j].setLine(line); // Set the line for the tram
            }
        }
    }

    public Line[] getLines() {
        return lines;
    }
    public Stop[] getStops() {
        return stops;
    }
    public int getNumberOfPassangers() {
        return numberOfPassengers;
    }
    public int getLengthOfSimulation() {
        return lengthOfSimulation;
    }
}
