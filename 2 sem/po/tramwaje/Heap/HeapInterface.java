package Heap;

// Interface defining operations for a heap
public interface HeapInterface {
    // Method to add an event to the heap
    void addEvent(Event event);

    // Method to get and remove the earliest event from the heap
    void getEvent();

    // Method to get and remove all events from the heap
    void getAllEvents();
}