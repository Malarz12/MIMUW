package cp2024.circuit;

public interface CircuitSolver {
    public CircuitValue solve(Circuit c);

    public void stop();
}
