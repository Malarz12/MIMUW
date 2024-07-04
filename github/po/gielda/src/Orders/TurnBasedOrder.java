package Orders;

import Investors.Investor;

// TurnBasedOrder class, extending ImmediateOrder.
public class TurnBasedOrder extends ImmediateOrder {

    // Constructor for initializing TurnBasedOrder.
    public TurnBasedOrder(int numberOfShares, int price, Investor investor, String share, int turn, int whichInTurn, boolean type, int lastTurn) {
        super(numberOfShares, price, investor, share, turn, whichInTurn, type); // Calling superclass constructor.
        setLastTurn(lastTurn); // Setting the last turn.
    }

    @Override
    public Order clone() {
        Investor temp = getInvestor().clone();
        Order result = new TurnBasedOrder(getNumberOfShares(), getPrice(), temp, getShare(), getTurn(), getWhichInTurn(), getType(), getLastTurn());
        return result;
    }
}