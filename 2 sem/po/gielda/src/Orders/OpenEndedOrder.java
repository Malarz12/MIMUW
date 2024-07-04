package Orders;

import Investors.Investor;
import Simulation.ShareSimulation;

// OpenEndedOrder class, extending Order.
public class OpenEndedOrder extends Order {

    // Constructor for initializing OpenEndedOrder.
    public OpenEndedOrder(int numberOfShares, int price, Investor investor, String share, int turn, int whichInTurn, boolean type) {
        super(numberOfShares, price, investor, share, turn, whichInTurn, type); // Calling superclass constructor.
    }

    // Override method to check if the order is possible.
    @Override
    public boolean isPossible(ShareSimulation shareSimulation) {
        // Check if the number of shares is less than or equal to 0.
        if (getNumberOfShares() <= 0) {
            if (getType() == false) {
                getInvestor().getWallet().put(getShare(), getInvestor().getWallet().get(getShare()) + getNumberOfShares());
            }
            return false; // Order is not possible.
        }
        return true; // Order is possible.
    }

    @Override
    public Order clone() {
        Investor temp = getInvestor().clone();
        Order result = new OpenEndedOrder(getNumberOfShares(), getPrice(), temp, getShare(), getTurn(), getWhichInTurn(), getType());
        return result;
    }
}
