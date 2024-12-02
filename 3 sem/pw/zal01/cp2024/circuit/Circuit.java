package cp2024.circuit;

public class Circuit {
    private final CircuitNode root;

    public Circuit(CircuitNode root) {
        this.root = root;
    }

    public final CircuitNode getRoot() {
        return this.root;
    }
}
