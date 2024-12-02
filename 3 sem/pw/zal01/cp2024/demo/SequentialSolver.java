package cp2024.demo;

import cp2024.circuit.*;

public class SequentialSolver implements CircuitSolver {
    private Boolean acceptComputations = true;

    @Override
    public CircuitValue solve(Circuit c) {
        boolean ret;

        if (!acceptComputations)
                return new BrokenCircuitValue();

        try {
            ret = recursiveSolve(c.getRoot());
        } catch (InterruptedException e) {
            return new BrokenCircuitValue();
        }
        return new ImmediateCircuitValue(ret);
    }

    @Override
    public void stop() {
        acceptComputations = false;
    }

    private boolean recursiveSolve(CircuitNode n) throws InterruptedException {
        if (n.getType() == NodeType.LEAF)
            return ((LeafNode) n).getValue();

        CircuitNode[] args = n.getArgs();

        return switch (n.getType()) {
            case IF -> solveIF(args);
            case AND -> solveAND(args);
            case OR -> solveOR(args);
            case GT -> solveGT(args, ((ThresholdNode) n).getThreshold());
            case LT -> solveLT(args, ((ThresholdNode) n).getThreshold());
            case NOT -> solveNOT(args);
            default -> throw new RuntimeException("Illegal type " + n.getType());
        };
    }

    private boolean solveNOT(CircuitNode[] args) throws InterruptedException {
        return !recursiveSolve(args[0]);
    }

    private boolean solveLT(CircuitNode[] args, int threshold) throws InterruptedException {
        int gotTrue = 0;
        for (CircuitNode arg : args) {
            if (recursiveSolve(arg))
                gotTrue++;
        }
        return gotTrue < threshold;
    }

    private boolean solveGT(CircuitNode[] args, int threshold) throws InterruptedException {
        int gotTrue = 0;
        for (CircuitNode arg : args) {
            if (recursiveSolve(arg))
                gotTrue++;
        }
        return gotTrue > threshold;
    }

    private boolean solveOR(CircuitNode[] args) throws InterruptedException {
        for (CircuitNode c : args) {
            if (recursiveSolve(c))
                return true;
        }
        return false;
    }

    private boolean solveAND(CircuitNode[] args) throws InterruptedException {
        for (CircuitNode c : args) {
            if (!recursiveSolve(c))
                return false;
        }
        return true;
    }

    private boolean solveIF(CircuitNode[] args) throws InterruptedException {
        boolean b = recursiveSolve(args[0]);
        return b ? recursiveSolve(args[1]) : recursiveSolve(args[2]);
    }
}
