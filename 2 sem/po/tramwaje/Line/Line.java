package Line;

import Randomization.Losowanie;
import ZTM.Stop;
import ZTM.Tram;
import Heap.Heap;
import Heap.TramArrival;

// Class representing a tram line
public class Line {
    private Route route; // Route of the tram line
    private int gap; // Time gap between trams
    private Tram[] trams; // Trams operating on the line
    private int index; // Index of the line

    // Constructor to initialize the line with stops, timetable, trams, and index
    public Line(Stop[] stops, int[] timeTable, Tram[] trams, int index) {
        this.route = new Route(stops, timeTable); // Create a new route
        this.index = index; // Set the index of the line
        this.trams = new Tram[trams.length]; // Initialize the array of trams
        for (int i = 0; i < trams.length; i++) {
            this.trams[i] = trams[i]; // Assign trams to the line
            if (i < trams.length / 2) {
                this.trams[i].setDirection(1); // Set direction for the trams
                this.trams[i].setStartingDirection(1);
            }
        }
        for (int i = 0; i < timeTable.length; i++) {
            gap = gap + timeTable[i] * 2; // Calculate the gap between trams
        }
        gap = gap / trams.length;
    }

    // Method to get the index of the line
    public int getIndex() {
        return index;
    }

    // Method to get the trams operating on the line
    public Tram[] getTrams() {
        return trams;
    }

    // Method to determine the destination stop of a tram based on its current stop and direction
    public Stop getGoal(Tram tram, Stop stop, int tempDirection) {
        if (tempDirection == 1) {
            int j = 0;
            while (route.getStops()[j] != stop) {
                j++;
            }
            j++;
            int fate = Losowanie.losuj(j, route.getStops().length - 1);

            return route.getStops()[fate];
        } else {
            int j = route.getStops().length - 1;
            while (route.getStops()[j] != stop) {
                j--;
            }
            j--;
            int fate = Losowanie.losuj(0, j);

            return route.getStops()[fate];
        }
    }

    // Method to get the index of a stop in the route in the direction of 1
    public int getIndexDir1(Stop stop) {
        for (int i = 0; i < route.getStops().length; i++) {
            if (route.getStops()[i] == stop) {
                return i;
            }
        }
        return 0;
    }

    // Method to get the index of a stop in the route in the direction of 0
    public int getIndexDir0(Stop stop) {
        for (int i = route.getStops().length - 1; i >= 0; i--) {
            if (route.getStops()[i] == stop) {
                return i;
            }
        }
        return 0;
    }

    // Method to get the route of the line
    public Route getRoute() {
        return route;
    }

    // Method to plan tram arrivals for the day and add them to the event heap
    public void planOfTheDay(Heap heap, int day) {
        TramArrival tramArrival; // Variable to hold tram arrival events
        int time; // Variable to track the time of tram arrivals

        // Iterate over each tram in the line
        for (int i = 0; i < trams.length; i++) {
            // Calculate initial time for tram arrivals based on tram index
            if (i < trams.length / 2) {
                time = 6 * 60 + i * gap; // Morning peak hour starting at 6:00 AM
            } else {
                time = 6 * 60 + (i - trams.length / 2) * gap; // Afternoon peak hour starting at 6:00 AM
            }

            // Continue planning tram arrivals until the end of the operating day
            while (time <= 23 * 60) {
                // Check the direction of the tram
                if (trams[i].getDirection() == 1) {
                    // If tram is moving forward, plan arrivals at each stop
                    tramArrival = new TramArrival(time, day, route.getStops()[0], trams[i], 1); // Create arrival event for the initial stop
                    heap.addEvent(tramArrival); // Add the event to the heap

                    // Plan arrivals for subsequent stops based on timetable
                    for (int j = 0; j < route.getTimeTable().length - 1; j++) {
                        time = time + route.getTimeTable()[j]; // Update time based on timetable
                        tramArrival = new TramArrival(time, day, route.getStops()[j + 1], trams[i], 1); // Create arrival event for next stop
                        heap.addEvent(tramArrival); // Add the event to the heap
                    }
                    trams[i].setDirection(0); // Set tram direction to reverse after reaching the last stop
                } else if (trams[i].getDirection() == 0) {
                    // If tram is moving backward, plan arrivals at each stop in reverse order
                    tramArrival = new TramArrival(time, day, route.getStops()[route.getStops().length - 1], trams[i], 0); // Create arrival event for the last stop
                    heap.addEvent(tramArrival); // Add the event to the heap

                    // Plan arrivals for previous stops based on timetable in reverse order
                    for (int j = route.getTimeTable().length - 2; j >= 0; j--) {
                        time = time + route.getTimeTable()[j]; // Update time based on timetable
                        tramArrival = new TramArrival(time, day, route.getStops()[j], trams[i], 0); // Create arrival event for previous stop
                        heap.addEvent(tramArrival); // Add the event to the heap
                    }
                    trams[i].setDirection(1); // Set tram direction to forward after reaching the first stop
                }
                time = time + route.getTimeTable()[route.getTimeTable().length - 1]; // Update time for the end of the loop
            }
            // in case the tram has not yet finished the designated route
            if (trams[i].getDirection() != trams[i].getStartingDirection() && time < 24 * 60) {
                if (trams[i].getDirection() == 1) {
                    tramArrival = new TramArrival(time, day, route.getStops()[0], trams[i], 1); // Create arrival event for the initial stop
                    heap.addEvent(tramArrival); // Add the event to the heap

                    // Plan arrivals for subsequent stops based on timetable
                    for (int j = 0; j < route.getTimeTable().length - 1; j++) {
                        time = time + route.getTimeTable()[j]; // Update time based on timetable
                        if (time > 24 * 60) {
                            break;
                        }
                        tramArrival = new TramArrival(time, day, route.getStops()[j + 1], trams[i], 1); // Create arrival event for next stop
                        heap.addEvent(tramArrival); // Add the event to the heap
                    }
                    trams[i].setDirection(0); // Set tram direction to reverse after reaching the last stop
                } else if (trams[i].getDirection() == 0) {
                    // If tram is moving backward, plan arrivals at each stop in reverse order
                    tramArrival = new TramArrival(time, day, route.getStops()[route.getStops().length - 1], trams[i], 0); // Create arrival event for the last stop
                    heap.addEvent(tramArrival); // Add the event to the heap

                    // Plan arrivals for previous stops based on timetable in reverse order
                    for (int j = route.getTimeTable().length - 2; j >= 0; j--) {
                        time = time + route.getTimeTable()[j]; // Update time based on timetable
                        if (time > 24 * 60) {
                            break;
                        }
                        tramArrival = new TramArrival(time, day, route.getStops()[j], trams[i], 0); // Create arrival event for previous stop
                        heap.addEvent(tramArrival); // Add the event to the heap
                    }
                    trams[i].setDirection(1); // Set tram direction to forward after reaching the first stop
                }
            }
        }

        // After planning tram arrivals for the day, reset tram directions for the next day
        for (int i = 0; i < trams.length; i++) {
            if (i < trams.length / 2) {
                trams[i].setDirection(1); // Set direction to forward for the first half of trams
            } else {
                trams[i].setDirection(0); // Set direction to backward for the second half of trams
            }
        }
    }
}