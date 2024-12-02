package cp2024.circuit;

/** A circuit node of type NOT, AND, OR, or IF. */
public final class SimpleNode extends CircuitNode {
    public SimpleNode(NodeType t, CircuitNode[] args) {
        super(t, args);
        if (t != NodeType.NOT && t != NodeType.AND && t != NodeType.OR  && t != NodeType.IF) {
            throw new IllegalArgumentException("Simple nodes must be of type NOT, AND, OR, or IF.");
        }
        if (t == NodeType.NOT && args.length != 1) {
            throw new IllegalArgumentException("NOT nodes must have exactly one argument.");
        }
        if ((t == NodeType.AND || t == NodeType.OR) && args.length < 2) {
            throw new IllegalArgumentException("AND and OR nodes must have at least two arguments.");
        }
        if (t == NodeType.IF && args.length != 3) {
            throw new IllegalArgumentException("IF nodes must have exactly three arguments.");
        }
    }
}
