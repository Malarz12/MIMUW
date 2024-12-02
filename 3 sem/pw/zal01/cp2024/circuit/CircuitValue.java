package cp2024.circuit;

/** Interface for a deferred (future) value returned by a CircuitSolver. */
public interface CircuitValue {
    public boolean getValue() throws InterruptedException;
}
