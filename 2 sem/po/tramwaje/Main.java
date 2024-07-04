import Simulation.Simulation;
import Simulation.DataCollector;

public class Main {
    public static void main(String[] args) {

        DataCollector dataCollector = new DataCollector();
        // Create a Simulation object with the provided parameters
        Simulation simulation = new Simulation(dataCollector.getLines(), dataCollector.getStops(), dataCollector.getNumberOfPassangers(), dataCollector.getLengthOfSimulation());
        simulation.go(); // Start the simulation
        simulation.getStatistics().printStatistics();
    }
}
