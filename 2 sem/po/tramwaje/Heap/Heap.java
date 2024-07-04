package Heap;

import Simulation.Statistics;
import java.util.Arrays;

// Implementation of HeapInterface
public class Heap implements HeapInterface {
    private Event[] queue; // Array to store events
    private int size; // Capacity of the heap
    private int trueSize; // Number of elements in the heap
    private Statistics statistics;
    public Heap(Statistics statistics) {
        this.size = 10;
        this.trueSize = 0;
        this.queue = new Event[this.size];
        this.statistics = statistics;

    }
    // Method to swap elements at indices i and j
    private void swap(int i, int j) {
        Event help = queue[i];
        queue[i] = queue[j];
        queue[j] = help;
    }

    // Method to add an event to the heap
    public void addEvent(Event event) {
        if (trueSize >= size - 1) { // If the heap is full, resize it
            size *= 2;
            queue = Arrays.copyOf(queue, size);
        }

        queue[++trueSize] = event; // Add the event to the end of the heap
        int index = trueSize;

        // Heapify the heap by moving the newly added event up
        while (index > 1 && queue[index].getTime() < queue[index / 2].getTime()) {
            swap(index, index / 2);
            index /= 2;
        }
    }

    // Method to restore heap properties starting from index i
    private void recover(int i) {
        int left = 2 * i;
        int right = 2 * i + 1;
        int min = i;

        // Find the minimum among the current node, left child, and right child
        if (left <= trueSize && queue[left].getTime() < queue[min].getTime()) {
            min = left;
        }
        if (right <= trueSize && queue[right].getTime() < queue[min].getTime()) {
            min = right;
        }

        // If the minimum is not the current node, swap and recover
        if (min != i) {
            swap(i, min);
            recover(min);
        }
    }

    // Method to get and remove the earliest event from the heap
    public void getEvent() {
        assert trueSize > 0: "Queue is empty."; // Assertion to check if the heap is not empty

        queue[1].message(this); // Print the message of the earliest event
        queue[1] = queue[trueSize--]; // Replace the root with the last element
        recover(1); // Restore heap properties
    }

    // Method to get and remove all events from the heap
    public void getAllEvents() {
        while (trueSize > 0) { // Keep getting events until the heap is empty
            getEvent();
        }
    }

    public Statistics getStatistics() {
        return statistics;
    }
}

