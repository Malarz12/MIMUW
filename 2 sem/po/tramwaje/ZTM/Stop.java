package ZTM;

// Class representing a stop in the public transportation system
public class Stop extends ZTM {
    private String name; // Name of the stop

    // Constructor to initialize the stop with capacity and name
    public Stop(int capacity, String name) {
        super(capacity, 0, null); // Call superclass constructor with capacity, initial number of passengers, and line
        this.name = name; // Set the name of the stop
    }

    // Method to get the name of the stop
    public String getName() {
        return name;
    }
}