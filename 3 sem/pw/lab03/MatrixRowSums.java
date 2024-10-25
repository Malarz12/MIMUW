import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.IntBinaryOperator;

public class MatrixRowSums {
    private static final int N_ROWS = 10;
    private static final int N_COLUMNS = 100;

    private static IntBinaryOperator matrixDefinition = (row, col) -> {
        int a = 2 * col + 1;
        return (row + 1) * (a % 4 - 2) * a;
    };

    private static void printRowSumsSequentially() {
        for (int r = 0; r < N_ROWS; ++r) {
            int sum = 0;
            for (int c = 0; c < N_COLUMNS; ++c) {
                sum += matrixDefinition.applyAsInt(r, c);
            }
            System.out.println(r + " -> " + sum);
        }
    }

    private static class MyRunnable implements Runnable {
        private final int column;
        private ConcurrentHashMap<Integer, Integer> sumMap;
        private ConcurrentHashMap<Integer, Integer> countMap;

        public MyRunnable(int column, ConcurrentHashMap<Integer, Integer> sumMap, ConcurrentHashMap<Integer, Integer> countMap) {
            this.column = column;
            this.sumMap = sumMap;
            this.countMap = countMap;
        }

        @Override
        public void run() {
            for (int i = 0; i < N_ROWS; i++) {
                int help = matrixDefinition.applyAsInt(i, column);
                sumMap.merge(i, help, Integer::sum);
                int inc = countMap.merge(i, 1, Integer::sum);
                if(inc == N_COLUMNS) {
                    System.out.println((i + 1)+ " -> " + sumMap.get(i));
                    sumMap.remove(i);
                    countMap.remove(i);
                }
            }
        }
    }


    private static void printRowSumsInParallel() throws InterruptedException {
        ConcurrentHashMap<Integer, Integer> countMap = new ConcurrentHashMap<>();
        for (int i = 0; i < N_ROWS; i++) {
            countMap.put(i, 0);
        }
        ConcurrentHashMap<Integer, Integer> sumMap = new ConcurrentHashMap<>();
        for (int i = 0; i < N_ROWS; i++) {
            sumMap.put(i, 0);
        }
        List<Thread> threads = new ArrayList<>();
        for (int c = 0; c < N_COLUMNS; ++c) {
            threads.add(new Thread((new MyRunnable(c, sumMap, countMap))));
        }
        for (Thread t : threads) {
            t.start();
        }

        try {
            for (Thread t : threads) {
                t.join();
            }
        } catch (InterruptedException e) {
            for (Thread t : threads) {
                t.interrupt();
            }
            throw e;
        }
    }

    public static void main(String[] args) {
        try {
            System.out.println("-- Sequentially --");
            printRowSumsSequentially();
            System.out.println("-- In parallel --");
            printRowSumsInParallel();
            System.out.println("-- End --");
        } catch (InterruptedException e) {
            System.err.println("Main interrupted.");
        }
    }
}
