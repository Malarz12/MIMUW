package Line;

import ZTM.Stop;

// Class representing a route with stops and timetable
public class Route {
    private Stop[] stops; // Array to store stops along the route
    private int[] timeTable; // Array to store timetable for the stops

    // Constructor to initialize the route with stops and timetable
    public Route(Stop[] stops, int[] timeTable) {
        // Initialize arrays for stops and timetable
        this.stops = new Stop[stops.length];
        this.timeTable = new int[timeTable.length];

        // Copy stops and timetable from parameters to class fields
        for (int i = 0; i < stops.length; i++) {
            this.stops[i] = stops[i];
            this.timeTable[i] = timeTable[i];
        }
    }

    // Method to get the timetable for the route
    public int[] getTimeTable() {
        return timeTable;
    }

    // Method to get the stops along the route
    public Stop[] getStops() {
        return stops;
    }
}