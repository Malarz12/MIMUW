package Passenger;

import Randomization.Losowanie;
import ZTM.Stop;
import ZTM.Tram;

// Class representing a passenger
public class Passenger {
    private int index; // Passenger index
    private Stop initialStop; // Initial stop where the passenger boards
    private Stop goal; // Goal stop where the passenger wants to go
    private Tram tram; // Tram the passenger is on
    private int startOfWaiting;
    // Constructor to initialize passenger with index and stops array
    public Passenger(int index, Stop[] stops) {
        this.index = index;

        // Randomly select an initial stop from the stops array
        int fate = Losowanie.losuj(0, stops.length - 1);
        this.initialStop = stops[fate];
        this.startOfWaiting = 0;
    }
    public void setStartOfWaiting(int time) {
        startOfWaiting = time;
    }
    public int getStartOfWaiting() {
        return startOfWaiting;
    }
    // Method to get the passenger index
    public int getIndex() {
        return index;
    }

    // Method to get the initial stop of the passenger
    public Stop getInitialStop() {
        return initialStop;
    }

    // Method to get the tram the passenger is on
    public Tram getTram() {
        return tram;
    }

    // Method for the passenger to board a tram at a stop
    public void getInToTram(Tram tram, Stop currentStop, int tempDirection) {
        tram.addPassenger(this); // Add passenger to the tram
        currentStop.deletePassenger(this); // Remove passenger from the current stop
        // Determine the goal stop for the passenger based on tram direction
        goal = tram.getLine().getGoal(tram, currentStop, tempDirection);
        this.tram = tram; // Set the tram for the passenger
    }

    // Method to get the goal stop of the passenger
    public Stop getGoal() {
        return goal;
    }
}