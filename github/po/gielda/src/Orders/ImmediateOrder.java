package Orders;

import Investors.Investor;
import Simulation.ShareSimulation;

// ImmediateOrder class, extending Order.
public class ImmediateOrder extends Order {
    private int lastTurn; // Last turn until which this order can be executed.

    // Constructor for initializing ImmediateOrder.
    public ImmediateOrder(int numberOfShares, int price, Investor investor, String share, int turn, int whichInTurn, boolean type) {
        super(numberOfShares, price, investor, share, turn, whichInTurn, type); // Calling superclass constructor.
        this.lastTurn = turn; // Setting last turn to the next turn after creation.
    }

    // Setter for updating the last turn.
    public void setLastTurn(int lastTurn) {
        this.lastTurn = lastTurn;
    }

    // Getter for retrieving the last turn.
    public int getLastTurn() {
        return lastTurn;
    }

    // Override method to check if the order is possible.
    @Override
    public boolean isPossible(ShareSimulation shareSimulation) {
        // Check if the turn of shareSimulation is greater than the last turn or if number of shares is less than or equal to 0.
        if (shareSimulation.getTurn() > lastTurn || getNumberOfShares() <= 0) {
            if (getType() == false) {
                getInvestor().getWallet().put(getShare(), getInvestor().getWallet().get(getShare()) + getNumberOfShares());
            }
            return false; // Order is not possible.
        } else {
            return true; // Order is possible.
        }
    }

    @Override
    public Order clone() {
        Investor temp = getInvestor().clone();
        Order result = new OpenEndedOrder(getNumberOfShares(), getPrice(), temp, getShare(), getTurn(), getWhichInTurn(), getType());
        return result;
    }
}
