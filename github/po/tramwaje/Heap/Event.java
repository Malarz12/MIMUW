package Heap;

import Simulation.Statistics;

// Abstract class representing an event
public abstract class Event {
    private int time;
    private int day;
    public Event(int time, int day) {
        this.time = time;
        this.day = day;
    }
    public int getTime() {
        return time;
    }

    public int getDay() {
        return day;
    }

    // Method to get the hour from the time
    public int getHour() {
        return time / 60;
    }

    // Method to get the minute from the time
    public int getMinute() {
        return time % 60;
    }

    // Abstract method to be implemented by subclasses to display a message
    public abstract void message(Heap heap);
}
