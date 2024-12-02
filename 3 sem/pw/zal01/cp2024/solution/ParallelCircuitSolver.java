package cp2024.solution;

import cp2024.circuit.*;
import java.util.ArrayList;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;

public class ParallelCircuitSolver implements CircuitSolver {
    private final ExecutorService mainExecutor;  // Main executor for parallel tasks
    private final AtomicBoolean stopAll;        // Flag to stop all threads

    public ParallelCircuitSolver() {
        // Initialize the executor as a ThreadPoolExecutor
        // It allows an unbounded number of threads to be created and used.
        mainExecutor = new ThreadPoolExecutor(
                0,
                Integer.MAX_VALUE,           // Min and max number of threads
                80,
                TimeUnit.MILLISECONDS,      // Time to wait before creating additional threads
                new SynchronousQueue<>()
        );
        stopAll = new AtomicBoolean(false);  // Initialize the stop flag
    }

    @Override
    public void stop() {
        // Stop all threads and shutdown the executor
        mainExecutor.shutdownNow();
        stopAll.set(true);  // Set the stop flag
    }

    @Override
    public CircuitValue solve(Circuit c) {
        // If the stop flag is set, return a "broken" value
        if (stopAll.get()) {
            return new ParallelBrokenCircuitValue();
        }
        try {
            // Start a recursive task in a separate thread to solve the circuit
            Future<Boolean> res = mainExecutor.submit(() -> recursion(c.getRoot()));
            // Return the result as a ParallelCircuitValue object
            return new ParallelCircuitValue(res);
        } catch (Exception e) {
            return new ParallelBrokenCircuitValue();
        }
    }

    private boolean recursion(CircuitNode c) throws InterruptedException {
        try {
            // If the node is a LEAF, return its value
            if (c.getType() == NodeType.LEAF)
                return ((LeafNode) c).getValue();

            CircuitNode[] args = c.getArgs(); // Get the arguments (children) of the node
            // Depending on the node type, solve the appropriate circuit type
            return switch (c.getType()) {
                case IF -> solveIF(args); // Solve for IF type
                case AND -> solveAO(args, false); // Solve for AND type
                case OR -> solveAO(args, true); // Solve for OR type
                case GT -> solveGT(args, ((ThresholdNode) c).getThreshold()); // Solve for GT (greater than threshold)
                case LT -> solveLT(args, ((ThresholdNode) c).getThreshold()); // Solve for LT (less than threshold)
                case NOT -> solveNOT(args); // Solve for NOT type
                default -> throw new RuntimeException("Illegal type " + c.getType()); // Error if the node type is unknown
            };
        } catch (Exception e) {
            Thread.currentThread().interrupt();  // Interrupt the thread in case of an error
            throw new InterruptedException();
        }
    }

    // Helper method to handle parallel processing for arguments
    private ArrayList<Future<Object>> futuresHelper(CircuitNode[] args, ExecutorCompletionService<Object> subExecutor) throws InterruptedException {
        ArrayList<Future<Object>> futures = new ArrayList<>(); // List to store the futures of tasks

        try {
            // For each argument node, submit a recursive task to a separate thread
            for (CircuitNode arg : args) {
                Future<Object> temp = subExecutor.submit(() -> recursion(arg));
                futures.add(temp);  // Add the task's future to the list
            }
            // Check if the thread has been interrupted, if so, cancel all tasks
            if (Thread.currentThread().isInterrupted()) {
                for (Future<Object> temp : futures) {
                    temp.cancel(true);
                }
                throw new InterruptedException();
            }
        } catch (Exception e) {
            // Cancel all tasks in case of an error
            for (Future<Object> temp : futures) {
                temp.cancel(true);
            }
            Thread.currentThread().interrupt(); // Interrupt the thread
            throw new InterruptedException();
        }
        return futures;
    }

    // Handles logical AND/OR operations
    private boolean solveAO(CircuitNode[] args, boolean param) throws InterruptedException {
        ExecutorCompletionService<Object> subExecutor = new ExecutorCompletionService<>(mainExecutor);  // Executor for parallel tasks
        ArrayList<Future<Object>> futures;

        try {
            // Start tasks for each argument node
            futures = futuresHelper(args, subExecutor);
        } catch (Exception e) {
            Thread.currentThread().interrupt();
            throw new InterruptedException();
        }

        try {
            for (int i = 0; i < futures.size(); ++i) {
                // Wait for one of the tasks to finish
                Future<Object> temp = subExecutor.take();
                // If the argument is True and the operation is OR, return True
                if ((Boolean) temp.get() && param) {
                    // If the operation is OR and True has been found, cancel all remaining tasks
                    for (Future<Object> temp2 : futures) {
                        temp2.cancel(true);
                    }
                    return true;
                } else if (!((Boolean) temp.get()) && !param) {
                    // If the operation is AND and False has been found, cancel all tasks
                    for (Future<Object> temp2 : futures) {
                        temp2.cancel(true);
                    }
                    return false;
                }
            }
        } catch (Exception e) {
            // Cancel all tasks in case of an error
            for (Future<Object> temp : futures) {
                temp.cancel(true);
            }
            Thread.currentThread().interrupt();
            throw new InterruptedException();
        }
        // Return the opposite of the parameter (True if AND and no False found, False if OR and no True found)
        return !param;
    }

    // This method solves the conditional "IF" operation in the circuit.
    private Boolean solveIF(CircuitNode[] args) throws InterruptedException {
        // Ensure that the "IF" operation has exactly 3 arguments
        if(args.length != 3) {
            Thread.currentThread().interrupt();  // Interrupt the current thread if the condition is not met
            throw new IllegalArgumentException(); // Throw an exception if the number of arguments is incorrect
        }

        // Create an ExecutorCompletionService to manage asynchronous tasks
        ExecutorCompletionService<Object> subExecutor = new ExecutorCompletionService<>(mainExecutor);

        // Declare Futures for the results of each of the 3 tasks
        Future<Object> firstResult = null;
        Future<Object> secondResult = null;
        Future<Object> thirdResult = null;

        try {
            // Submit the tasks (solving the recursion for each argument) to the Executor
            firstResult = subExecutor.submit(() -> recursion(args[0]));
            secondResult = subExecutor.submit(() -> recursion(args[1]));
            thirdResult = subExecutor.submit(() -> recursion(args[2]));
        } catch (Exception e) {
            // If an exception occurs during task submission, cancel all running tasks
            if(firstResult != null) firstResult.cancel(true);
            if(secondResult != null) secondResult.cancel(true);
            if(thirdResult != null) thirdResult.cancel(true);

            // Interrupt the current thread and throw an InterruptedException
            Thread.currentThread().interrupt();
            throw new InterruptedException();
        }

        // Initialize boolean variables to store the values of the arguments
        boolean firstValue = false, secondValue = false, thirdValue = false;
        // Flags to check if the values for each argument are known
        boolean firstKnown = false, secondKnown = false, thirdKnown = false;

        try {
            // Loop to check the results of the three tasks
            for (int i = 0; i < 3; ++i) {
                // If the current thread is interrupted, cancel all tasks and throw InterruptedException
                if (Thread.currentThread().isInterrupted()) {
                    firstResult.cancel(true);
                    secondResult.cancel(true);
                    thirdResult.cancel(true);
                    throw new InterruptedException();
                }

                // Take the next completed task from the Executor
                Future<Object> temp = subExecutor.take();

                // If the task was cancelled, skip it
                if (temp.isCancelled()) {
                    continue;
                }

                // Assign the result from the task to the corresponding variable
                if (temp == firstResult) {
                    firstValue = (Boolean) temp.get(); // Get the result for the first argument
                    firstKnown = true;
                } else if (temp == secondResult) {
                    secondValue = (Boolean) temp.get(); // Get the result for the second argument
                    secondKnown = true;
                } else if (temp == thirdResult) {
                    thirdValue = (Boolean) temp.get(); // Get the result for the third argument
                    thirdKnown = true;
                }

                // If both second and third values are known and are equal, return the second value
                if (secondKnown && thirdKnown && (secondValue == thirdValue)) {
                    firstResult.cancel(true);
                    secondResult.cancel(true);
                    thirdResult.cancel(true);
                    return secondValue;  // The result is the second value, as the third is the same
                } else if (firstKnown && firstValue) {
                    // If the first value is true, cancel the third task and return the second value if known
                    thirdResult.cancel(true);
                    firstResult.cancel(true);
                    if (secondKnown) return secondValue;
                } else if (firstKnown && !firstValue) {
                    // If the first value is false, cancel the second task and return the third value if known
                    firstResult.cancel(true);
                    secondResult.cancel(true);
                    if (thirdKnown) return thirdValue;
                }
            }
        } catch (InterruptedException | ExecutionException e) {
            // If an exception occurs during execution, cancel all tasks
            firstResult.cancel(true);
            secondResult.cancel(true);
            thirdResult.cancel(true);
            Thread.currentThread().interrupt();
            throw new InterruptedException();
        }

        // Default return value if no decision could be made within the loop
        return true;
    }
    // Solves for the NOT operation, which negates the value of the argument
    private Boolean solveNOT(CircuitNode[] args) throws InterruptedException {
        if (args.length != 1) { // A NOT node should have exactly one argument
            Thread.currentThread().interrupt();
            throw new IllegalArgumentException();
        }
        try {
            // Recursively solve the argument and negate its result
            return !(recursion(args[0]));
        } catch (Exception e) {
            // If there's an error, throw InterruptedException
            throw new InterruptedException();
        }
    }

    // Solves for a "less than" (LT) threshold operation
    private boolean solveLT(CircuitNode[] args, int x) throws InterruptedException {
        // If the number of arguments is less than the threshold x, return true (default behavior)
        if (args.length < x) return true;
        if (x == 0) return false; // If x is 0, return false (since no threshold is met)

        ExecutorCompletionService<Object> subExecutor = new ExecutorCompletionService<>(mainExecutor);  // Executor for parallel tasks
        ArrayList<Future<Object>> futures;

        try {
            // Start tasks for each argument
            futures = futuresHelper(args, subExecutor);
        } catch (Exception e) {
            Thread.currentThread().interrupt();
            throw new InterruptedException();
        }
        int counter = 0;  // Counter for how many True values are found
        int computed = 0; // Counter for how many tasks have been computed

        try {
            // Process the results from the tasks
            for (int i = 0; i < futures.size(); ++i) {
                Future<Object> temp = subExecutor.take();  // Wait for a task to finish
                computed++; // Increment the computed counter
                if ((Boolean) temp.get()) counter++;  // Increment the counter if the result is True

                // If the counter reaches the threshold x, cancel the remaining tasks and return false
                if (counter >= x) {
                    for (Future<Object> temp2 : futures) {
                        temp2.cancel(true);
                    }
                    return false;
                }

                // If it's impossible to reach the threshold with the remaining tasks, return true
                if (counter + futures.size() - computed < x) {
                    for (Future<Object> temp2 : futures) {
                        temp2.cancel(true);
                    }
                    return true;
                }
            }
        } catch (Exception e) {
            // Cancel all tasks in case of an error
            for (Future<Object> temp : futures) {
                temp.cancel(true);
            }
            Thread.currentThread().interrupt();
            throw new InterruptedException();
        }
        // If not enough True values were found to meet the threshold, return true
        return true;
    }

    // Solves for a "greater than" (GT) threshold operation
    private boolean solveGT(CircuitNode[] args, int x) throws InterruptedException {
        // If the number of arguments is less than or equal to x, return false (no threshold reached)
        if (args.length <= x) return false;

        ExecutorCompletionService<Object> subExecutor = new ExecutorCompletionService<>(mainExecutor);  // Executor for parallel tasks
        ArrayList<Future<Object>> futures;

        try {
            // Start tasks for each argument
            futures = futuresHelper(args, subExecutor);
        } catch (Exception e) {
            Thread.currentThread().interrupt();
            throw new InterruptedException();
        }
        int counter = 0;  // Counter for how many True values are found
        int computed = 0; // Counter for how many tasks have been computed

        try {
            // Process the results from the tasks
            for (int i = 0; i < futures.size(); ++i) {
                Future<Object> temp = subExecutor.take();  // Wait for a task to finish
                computed++; // Increment the computed counter
                if ((Boolean) temp.get()) counter++;  // Increment the counter if the result is True

                // If the counter exceeds the threshold x, cancel the remaining tasks and return true
                if (counter > x) {
                    for (Future<Object> temp2 : futures) {
                        temp2.cancel(true);
                    }
                    return true;
                }

                // If it's impossible to exceed the threshold with the remaining tasks, return false
                if (counter + futures.size() - computed <= x) {
                    for (Future<Object> temp2 : futures) {
                        temp2.cancel(true);
                    }
                    return false;
                }
            }
        } catch (Exception e) {
            // Cancel all tasks in case of an error
            for (Future<Object> temp : futures) {
                temp.cancel(true);
            }
            Thread.currentThread().interrupt();
            throw new InterruptedException();
        }
        // If not enough True values were found to exceed the threshold, return false
        return false;
    }
}
//ale essa