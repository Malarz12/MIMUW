package cp2024.demo;

import cp2024.circuit.CircuitValue;

public class ImmediateCircuitValue implements CircuitValue {
    private final boolean value;

    public ImmediateCircuitValue(boolean value) {
        this.value = value;
    }

    @Override
    public boolean getValue() {
        return this.value;
    }
}
