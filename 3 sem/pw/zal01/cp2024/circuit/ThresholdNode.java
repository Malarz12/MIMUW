package cp2024.circuit;

public final class ThresholdNode extends CircuitNode {
    public final int threshold;

    public ThresholdNode(NodeType t, CircuitNode[] args, int threshold) {
        super(t, args);
        if (args.length < 1) {
            throw new IllegalArgumentException("Threshold nodes must have at least one argument.");
        }
        if (threshold < 0) {
            throw new IllegalArgumentException("The threshold must be non-negative.");
        }
        if (t != NodeType.GT && t != NodeType.LT) {
            throw new IllegalArgumentException("Threshold nodes must be of type GT or LT.");
        }
        this.threshold = threshold;
    }

    public final int getThreshold() {
        return this.threshold;
    }
}
