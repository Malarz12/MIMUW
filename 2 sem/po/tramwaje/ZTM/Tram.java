package ZTM;

import Line.Line;

// Class representing a tram in the public transportation system
public class Tram extends ZTM {
    private int number; // Tram number
    private Line line; // Line to which the tram belongs
    private int direction; // Direction of the tram
    private int startingDirection; // Constructor to initialize the tram with capacity and number
    public Tram(int capacity, int number) {
        super(capacity, 0, null); // Call superclass constructor with capacity, initial number of passengers, and line
        this.line = null; // Initialize line to null
        this.number = number; // Set the number of the tram
        this.direction = 0; // Set the direction of the tram
        this.startingDirection = 0;
    }
    public void setStartingDirection(int i) {
        startingDirection = i;
    }
    public int getStartingDirection() {
        return startingDirection;
    }
    // Method to get the number of the tram
    public int getNumber() {
        return number;
    }

    // Method to get the line to which the tram belongs
    public Line getLine() {
        return line;
    }

    // Method to get the direction of the tram
    public int getDirection() {
        return direction;
    }

    // Method to set the line to which the tram belongs
    public void setLine(Line line) {
        this.line = line;
    }

    // Method to set the direction of the tram
    public void setDirection(int direction) {
        this.direction = direction;
    }
}