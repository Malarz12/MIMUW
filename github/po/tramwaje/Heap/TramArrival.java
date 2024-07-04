package Heap;

import Passenger.Passenger;
import Simulation.Statistics;
import ZTM.Stop;
import ZTM.Tram;

// Class representing an event of tram arrival
public class TramArrival extends Event {
    private Stop stop; // Stop where tram arrives
    private Tram tram; // Tram arriving
    private int tempDirection; // Temporary direction of the tram

    // Constructor to initialize tram arrival event
    public TramArrival(int time, int day, Stop stop, Tram tram, int tempDirection) {
        super(time, day); // Call superclass constructor
        this.stop = stop;
        this.tram = tram;
        this.tempDirection = tempDirection;
    }

    // Method to display a message based on tram arrival
    @Override
    public void message(Heap heap) {
        // Print tram arrival message
        System.out.println(getDay() + ", " + getHour() + ":" + getMinute() + " Przyjechal tramwaj nr.: " +
                tram.getNumber() + " Lini: " + tram.getLine().getIndex() + " na przystanek: " +
                stop.getName());

        // Check passengers exiting the tram
        for (int i = 0; i < tram.getCapacity(); i++) {
            if (tram.getPassengers()[i] != null && tram.getPassengers()[i].getGoal() == stop && stop.getCurrentNumber() < stop.getCapacity()) {
                // If passenger's goal is the current stop and there's capacity, passenger exits the tram
                stop.addPassenger(tram.getPassengers()[i]);
                System.out.println(getDay() + ", " + getHour() + ":" + getMinute() + " Pasazer: " + tram.getPassengers()[i].getIndex() +
                        " wysiadl z tramwaju " + tram.getNumber() + " Lini: " + tram.getLine().getIndex() + " na przystanek: " +
                        stop.getName());
                heap.getStatistics().increaseNumberOfHalts(getDay());
                tram.getPassengers()[i].setStartOfWaiting(getTime());
                tram.deletePassenger(tram.getPassengers()[i]);
            }
        }
        // Check passengers boarding the tram
        for (int i = 0; i < stop.getCapacity(); i++) {
            if (stop.getPassengers()[i] != null && tram.getCapacity() > tram.getCurrentNumber()) {
                // If there are passengers at the stop and tram has capacity, passengers board the tram
                if ((tempDirection == 1 && tram.getLine().getIndexDir1(stop) != tram.getLine().getRoute().getStops().length - 1) ||
                        (tempDirection == 0 && tram.getLine().getIndexDir0(stop) != 0)) {
                    System.out.print(getDay() + ", " + getHour() + ":" + getMinute() + " Pasazer: " + stop.getPassengers()[i].getIndex() +
                            " wsiadl do tramwaju " + tram.getNumber() + " Lini: " + tram.getLine().getIndex() + " z przystanku : " +
                            stop.getName());

                    heap.getStatistics().increaseTimeOfWaiting(getDay(),getTime() - stop.getPassengers()[i].getStartOfWaiting());
                    Passenger passenger = stop.getPassengers()[i];
                    stop.getPassengers()[i].getInToTram(tram, stop, tempDirection);
                    System.out.println(" z zamiarem wyścia na przystanku: " + passenger.getGoal().getName());
                    heap.getStatistics().increaseNumberOfTips(getDay());
                }
            }
        }
    }
}