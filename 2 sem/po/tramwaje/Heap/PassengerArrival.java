package Heap;

import Passenger.Passenger;
import Simulation.Statistics;

// Class representing an event of passenger arrival
public class PassengerArrival extends Event {
    private Passenger passenger; // Passenger arriving

    // Constructor to initialize passenger arrival event
    public PassengerArrival(int time, int day, Passenger passenger) {
        super(time, day); // Call superclass constructor
        this.passenger = passenger;
    }

    // Method to display a message based on passenger arrival
    @Override
    public void message(Heap heap) {
        // Check if there is capacity at the initial stop
        if (passenger.getInitialStop().getCurrentNumber() < passenger.getInitialStop().getCapacity()) {
            // If there is capacity, add the passenger to the initial stop
            passenger.getInitialStop().addPassenger(passenger);
            System.out.println(getDay() + ", " + getHour() + ":" + getMinute() + " Pasazer nr.: " + passenger.getIndex() +
                    " przyszedl na przystanek " + passenger.getInitialStop().getName());
            passenger.setStartOfWaiting(getTime());
            heap.getStatistics().increaseNumberOfHalts(getDay());
        } else {
            // If there is no capacity, inform the passenger and simulate returning home
            System.out.println(getDay() + ", " + getHour() + ":" + getMinute() + " Pasazer nr.: " + passenger.getIndex() +
                    " przyszedl na przystanek " + passenger.getInitialStop().getName() + " i z powodu braku miejsca wrocil do domu");
        }
    }
}