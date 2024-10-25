import java.util.ArrayList;
import java.util.Arrays;
import java.util.Random;

import static java.lang.Math.ceil;
import static java.lang.Math.min;

@SuppressWarnings("unused")
public class Vector {
    private static final int SUM_CHUNK_LENGTH = 10;
    private static final int DOT_CHUNK_LENGTH = 10;

    private final int[] elements;

    public Vector(int length) {
        elements = new int[length];
    }

    public Vector(int[] elements) {
        this.elements = Arrays.copyOf(elements, elements.length);
    }

    private final Vector sumSequential(Vector other) {
        if (this.elements.length != other.elements.length) {
            throw new IllegalArgumentException("Vector lengths differ.");
        }
        Vector result = new Vector(this.elements.length);
        for (int i = 0; i < result.elements.length; ++i) {
            result.elements[i] = this.elements[i] + other.elements[i];
        }
        return result;
    }

    private final int dotSequential(Vector other) {
        if (this.elements.length != other.elements.length) {
            throw new IllegalArgumentException("Vector lengths differ.");
        }
        int result = 0;
        for (int i = 0; i < this.elements.length; ++i) {
            result += this.elements[i] * other.elements[i];
        }
        return result;
    }

    private static class SumHelper implements Runnable {
        private final Vector left;
        private final Vector right;
        private final Vector result;
        private final int begin;
        private final int end;

        public SumHelper(Vector left, Vector right, Vector result, int begin, int end) {
            this.left = left;
            this.right = right;
            this.result = result;
            this.begin = begin;
            this.end = end;
        }

        @Override
        public void run() {
            try {
                for (int i = begin; i < end; i++) {
                    result.elements[i] = left.elements[i] + right.elements[i];
                }
            } catch (Exception e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    public Vector sum(Vector v) throws InterruptedException {
        if (elements.length != v.elements.length) {
            throw new IllegalArgumentException("Vector lengths differ.");
        }
        Vector result = new Vector(elements.length);

        ArrayList<Thread> threads = new ArrayList<>();
        int numberOfChunks = (int) Math.ceil((double)elements.length / 10);

        for (int i = 0; i < numberOfChunks; i++) {
            Runnable r = new SumHelper(this, v, result, i * 10, min(elements.length, (i + 1) * 10));
            threads.add(new Thread(r));
            threads.getLast().start();
        }

        for (int i = 0; i < numberOfChunks; i++) {
            try {
                threads.get(i).join();
            } catch (InterruptedException e) {
                for (Thread t : threads) {
                    t.interrupt();
                }
                throw e;
            }
        }

        return result;
    }

    private static class DotHelper implements Runnable {
        private final Vector left;
        private final Vector right;
        private final int begin;
        private final int end;
        private final int[] result;
        private final int resultPosition;

        public DotHelper(Vector left, Vector right, int begin, int end, int[] result, int resultPosition) {
            this.left = left;
            this.right = right;
            this.begin = begin;
            this.end = end;
            this.result = result;
            this.resultPosition = resultPosition;
        }

        @Override
        public void run() {
            try {
                int total = 0;
                for (int i = begin; i < end; i++) {
                    total = left.elements[i] * right.elements[i] + total;
                }
                result[resultPosition] = total;
            } catch (Exception e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    public int dot(Vector v) throws InterruptedException {
        if (elements.length != v.elements.length) {
            throw new IllegalArgumentException("Vector lengths differ.");
        }

        int nThreads = (int) Math.ceil((double) elements.length / 10);
        ArrayList<Thread> threads = new ArrayList<>();
        int[] partialResults = new int[nThreads];
        int total = 0;

        for (int i = 0; i < nThreads; i++) {
            Runnable r = new DotHelper(this, v, i * 10, Math.min((i + 1) * 10, elements.length), partialResults, i);
            Thread t = new Thread(r);
            threads.add(t);
            threads.getLast().start();
        }

        for (int i = 0; i < nThreads; i++) {
            try {
                threads.get(i).join();
            } catch (InterruptedException e) {
                for (Thread t : threads) {
                    t.interrupt();
                }
                throw e;
            }
        }

        for (int i = 0; i < nThreads; i++) {
            total = total + partialResults[i];
        }
        return total;
    }

    // ----------------------- TESTS -----------------------

    @Override
    public String toString() {
        return Arrays.toString(elements);
    }

    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof Vector)) {
            return false;
        }
        Vector other = (Vector) obj;
        return Arrays.equals(this.elements, other.elements);
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(this.elements);
    }

    private static final Random random = new Random();

    private static Vector generateRandomVector(int length) {
        int[] a = new int[length];
        for (int i = 0; i < length; ++i) {
            a[i] = random.nextInt(10);
        }
        return new Vector(a);
    }

    public static void main(String[] args) {
        try {
            for (int length : new int[] { 33, 35 }) {
                Vector a = generateRandomVector(length);
                System.out.println("A:        " + a);
                Vector b = generateRandomVector(length);
                System.out.println("B:        " + b);

                Vector c = a.sum(b);
                Vector cSequential = a.sumSequential(b);

                if (!c.equals(cSequential)) {
                    System.out.println("Sum error!");
                    System.out.println("Expected: " + cSequential);
                    System.out.println("Got:      " + c);
                } else {
                    System.out.println("Sum OK:   " + c);
                }

                int d = a.dot(b);
                int dotSequential = a.dotSequential(b);
                if (d != dotSequential) {
                    System.out.println("Dot error! Expected " + dotSequential + ", got " + d + ".");
                } else {
                    System.out.println("Dot OK: " + d);
                }
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.err.println("computations interrupted");
        }
    }
}
