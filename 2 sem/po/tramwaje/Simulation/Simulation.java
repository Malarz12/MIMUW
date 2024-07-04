package Simulation;

import Heap.PassengerArrival;
import ZTM.Stop;
import Passenger.Passenger;
import Line.Line;
import java.util.Arrays;
import Randomization.Losowanie;
import Heap.Heap;

// Class representing a simulation of tram operations
public class Simulation {
    private Line[] Lines; // Array to store tram lines in the simulation
    private int numberOfPassengers; // Number of passengers in the simulation
    private Stop[] stops; // Array to store stops in the simulation
    private Passenger[] passengers; // Array to store passengers in the simulation
    private int lengthOfSimulation; // Length of the simulation in days
    private Statistics statistics;
    // Constructor to initialize the simulation with lines, stops, number of passengers, and simulation length
    public Simulation(Line[] Lines, Stop[] stops, int numberOfPassengers, int lengthOfSimulation) {
        // Copy arrays and other parameters
        this.Lines = Arrays.copyOf(Lines, Lines.length);
        this.stops = Arrays.copyOf(stops, stops.length);
        this.numberOfPassengers = numberOfPassengers;
        this.lengthOfSimulation = lengthOfSimulation;
        this.passengers = new Passenger[numberOfPassengers]; // Initialize passenger array
        this.statistics = new Statistics(lengthOfSimulation);
    }

    // Method to start the simulation
    public void go() {
        // Generate passengers at random stops
        for(int i = 0; i < numberOfPassengers; i++) {
            passengers[i] = new Passenger(i, stops);
        }

        // Run simulation for each day
        for(int i = 0; i < lengthOfSimulation; i++) {
            Heap heap = new Heap(statistics); // Create a heap to store events
            // Generate passenger arrivals for the day and add them to the event heap
            for(int j = 0; j < numberOfPassengers; j++) {
                PassengerArrival help = new PassengerArrival(Losowanie.losuj(6 * 60, 12 * 60), i, passengers[j]);
                heap.addEvent(help);
            }

            // Plan tram operations for each line for the day
            for(int j = 0; j < Lines.length; j++) {
                Lines[j].planOfTheDay(heap,i);
            }

            // Execute all events in the event heap
            heap.getAllEvents();
            // Clean up trams after each day
            for(int j = 0; j < Lines.length; j++) {
                for(int k = 0; k < Lines[j].getTrams().length; k++) {
                    Lines[j].getTrams()[k].cleanUp();
                }
            }

            // Clean up stops after each day
            for(int j = 0; j < stops.length; j++) {
                for(int k = 0; k < stops[j].getCapacity(); k++) {
                    if(stops[j].getPassengers()[k] != null) {
                        statistics.increaseNumberOfHalts(i);
                        statistics.increaseTimeOfWaiting(i,24 * 60 - stops[j].getPassengers()[k].getStartOfWaiting());
                    }
                }
                stops[j].cleanUp();
            }
        }
    }
    public Statistics getStatistics() {
        return statistics;
    }
}
