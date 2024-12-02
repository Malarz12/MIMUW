package cp2024.demo;

import cp2024.circuit.CircuitValue;

public class BrokenCircuitValue implements CircuitValue {
    @Override
    public boolean getValue() throws InterruptedException {
        throw new InterruptedException();
    }
}
