package cp2024.circuit;

import java.time.Duration;

public abstract sealed class CircuitNode permits SimpleNode, LeafNode, ThresholdNode {
    private final NodeType type;
    private final CircuitNode[] args;

    public final NodeType getType() {
        return this.type;
    }

    public CircuitNode[] getArgs() throws InterruptedException {
        return this.args;
    }

    protected CircuitNode(NodeType t, CircuitNode[] args) {
        this.type = t;
        this.args = args;
    }

    /** Convenience function for creating circuit nodes. See demo. */
    public static CircuitNode mk(NodeType type, CircuitNode... args) {
        return switch (type) {
            case IF -> new SimpleNode(type, args);
            case AND -> new SimpleNode(type, args);
            case OR -> new SimpleNode(type, args);
            case NOT -> new SimpleNode(type, args);
            default -> throw new RuntimeException("Illegal type " + type);
        };
    }

    public static CircuitNode mk(NodeType type, int threshold, CircuitNode... args) {
        return switch (type) {
            case GT -> new ThresholdNode(type, args, threshold);
            case LT -> new ThresholdNode(type, args, threshold);
            default -> throw new RuntimeException("Illegal type " + type);
        };
    }

    public static CircuitNode mk(boolean value) {
        return new SleepyLeafNode(value, Duration.ZERO);
    }

    public static CircuitNode mk(boolean value, Duration duration) {
        return new SleepyLeafNode(value, duration);
    }
}
