import java.util.ArrayList;
import java.util.Vector;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.function.IntBinaryOperator;

public class VectorStream {
    private static final int STREAM_LENGTH = 10;
    private static final int VECTOR_LENGTH = 100;

    /**
     * Function that defines how vectors are computed: the i-th element depends on
     * the previous sum and the index i.
     * The sum of elements in the previous vector is initially given as zero.
     */
    private final static IntBinaryOperator vectorDefinition = (previousSum, i) -> {
        int a = 2 * i + 1;
        return (previousSum / VECTOR_LENGTH + 1) * (a % 4 - 2) * a;
    };

    private static void computeVectorStreamSequentially() {
        int[] vector = new int[VECTOR_LENGTH];
        int sum = 0;
        for (int vectorNo = 0; vectorNo < STREAM_LENGTH; ++vectorNo) {
            for (int i = 0; i < VECTOR_LENGTH; ++i) {
                vector[i] = vectorDefinition.applyAsInt(sum, i);
            }
            sum = 0;
            for (int x : vector) {
                sum += x;
            }
            System.out.println(vectorNo + " -> " + sum);
        }
    }
    public static class Vector {
        public int sum;
        public int[] elements;
        public Vector() {
            sum = 0;
            this.elements = new int[VECTOR_LENGTH];
        }
    }
    public static class MyRunnable implements Runnable {
        private CyclicBarrier barrier;
        private Vector vector;
        int index;
        public MyRunnable(int index, CyclicBarrier barrier, Vector vector) {
            this.index = index;
            this.vector = vector;
            this.barrier = barrier;
        }

        @Override
        public void run() {
            try {
                for (int i = 0; i < STREAM_LENGTH; i++) {
                    int help = vector.elements[index];
                    vector.elements[index] = vectorDefinition.applyAsInt(vector.sum, index);
                    barrier.await();
                    vector.sum = vector.sum - help + vector.elements[index];
                    barrier.await();
                    if(index == 0) {
                        System.out.println(i + " -> " + vector.sum);
                    }
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            } catch (BrokenBarrierException e) {
                throw new RuntimeException(e);
            }
        }
    }
    private static void computeVectorStreamInParallel() throws InterruptedException {
        CyclicBarrier barrier = new CyclicBarrier(VECTOR_LENGTH);
        Vector vector = new Vector();
        ArrayList<Thread> threads = new ArrayList<>();
        for (int vectorNo = 0; vectorNo < VECTOR_LENGTH; ++vectorNo) {
            threads.add(new Thread(new MyRunnable(vectorNo, barrier, vector)));
            threads.getLast().start();
        }

        for (int i = 0; i < threads.size(); i++) {
            threads.get(i).join();
        }
    }

    public static void main(String[] args) {
        try {
            System.out.println("-- Sequentially --");
            computeVectorStreamSequentially();
            System.out.println("-- Parallel --");
            computeVectorStreamInParallel();
            System.out.println("-- End --");
        } catch (InterruptedException e) {
            System.err.println("Main interrupted.");
        }
    }
}
