package ZTM;

import Passenger.Passenger;

// Abstract class representing a component in the public transportation system
public abstract class ZTM {
    private int capacity; // Capacity of the component
    private int currentNumber; // Current number of passengers in the component
    private Passenger[] passengers; // Array to store passengers

    // Constructor to initialize the component with capacity, current number of passengers, and passenger array
    public ZTM(int capacity, int currentNumber, Passenger[] passengers) {
        this.capacity = capacity; // Set the capacity
        this.currentNumber = currentNumber; // Set the current number of passengers

        // Initialize the passenger array
        this.passengers = new Passenger[capacity];

        // Set all elements of the passenger array to null
        for(int i = 0; i < this.passengers.length; i++) {
            this.passengers[i] = null;
        }
    }

    // Method to get the capacity of the component
    public int getCapacity() {
        return capacity;
    }

    // Method to get the current number of passengers in the component
    public int getCurrentNumber() {
        return currentNumber;
    }

    // Method to get the array of passengers in the component
    public Passenger[] getPassengers() {
        return passengers;
    }

    // Method to add a passenger to the component
    public void addPassenger(Passenger passenger) {
        // Iterate through the passenger array
        for(int i = 0; i < passengers.length; i++) {
            // If a null slot is found, add the passenger and increment the current number of passengers
            if(passengers[i] == null) {
                passengers[i] = passenger;
                currentNumber++;
                break; // Exit the loop after adding the passenger
            }
        }
    }

    // Method to delete a passenger from the component
    public void deletePassenger(Passenger passenger) {
        // Iterate through the passenger array
        for(int i = 0; i < passengers.length; i++) {
            // If the passenger is found, remove it and decrement the current number of passengers
            if(passengers[i] == passenger) {
                passengers[i] = null;
                currentNumber--;
                break; // Exit the loop after deleting the passenger
            }
        }
    }
    public void getData() {
        System.out.print(currentNumber +" " + capacity + " ");
        for(int i = 0; i < passengers.length; i++) {
            if(passengers[i] != null) System.out.print(passengers[i].getIndex() + " ");
            else System.out.print("null ");
        }
        System.out.println();
    }
    // Method to clean up the component (reset current number of passengers and clear the passenger array)
    public void cleanUp() {
        currentNumber = 0; // Reset the current number of passengers
        // Set all elements of the passenger array to null
        for(int i = 0; i < passengers.length; i++) {
            passengers[i] = null;
        }
    }
}

