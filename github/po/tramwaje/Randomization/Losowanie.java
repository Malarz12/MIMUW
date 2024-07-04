package Randomization;

import java.util.Random;

// Class for generating random numbers
public class Losowanie {

    // Method to generate a random integer between a lower and upper bound (inclusive)
    public static int losuj(int dolna, int gorna) {
        Random random = new Random(); // Create a new Random object
        return random.nextInt(gorna - dolna + 1) + dolna; // Generate and return a random number within the specified range
    }
}