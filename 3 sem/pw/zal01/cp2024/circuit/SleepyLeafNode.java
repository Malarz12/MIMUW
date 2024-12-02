package cp2024.circuit;

import java.time.Duration;

public class SleepyLeafNode extends LeafNode {
    private final boolean value;
    private final Duration delay;

    public SleepyLeafNode(boolean value) {
        this.value = value;
        this.delay = Duration.ZERO;
    }

    public SleepyLeafNode(boolean value, Duration delay) {
        this.value = value;
        this.delay = delay;
    }

    @Override
    public boolean getValue() throws InterruptedException {
        Thread.sleep(this.delay.toMillis());
        return this.value;
    }
}
