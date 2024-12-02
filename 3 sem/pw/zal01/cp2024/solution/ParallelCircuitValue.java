package cp2024.solution;

import cp2024.circuit.CircuitValue;

import java.util.concurrent.Future;

public class ParallelCircuitValue implements CircuitValue {

    Future<Boolean> res;
    public ParallelCircuitValue(Future<Boolean> res) {
        this.res = res;
    }
    @Override
    public boolean getValue() throws InterruptedException {
        try {
            return res.get();
        } catch (Exception e) {
            throw new InterruptedException("KKU");
        }
    }
}
