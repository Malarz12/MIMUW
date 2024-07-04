package Orders;

import Investors.Investor;
import Queue.Queue;
import Simulation.ShareSimulation;

// Order abstract class.
public abstract class Order {
    private int numberOfShares; // Number of shares for this order.
    private int price; // Price per share for this order.
    private Investor investor; // Investor placing the order.
    private String share; // Share symbol for this order.
    private int turn; // Turn in which the order is placed.
    private int whichInTurn; // Position of the order within the turn.
    private boolean type; // Type of the order: true - purchase, false - sale.

    // Constructor for initializing Order.
    public Order(int numberOfShares, int price, Investor investor, String share, int turn, int whichInTurn, boolean type) {
        this.share = share; // Initialize share.
        this.investor = investor; // Initialize investor.
        this.price = price; // Initialize price.
        this.numberOfShares = numberOfShares; // Initialize number of shares.
        this.turn = turn; // Initialize turn.
        this.whichInTurn = whichInTurn; // Initialize whichInTurn.
        this.type = type; // Initialize type.
    }

    // Funcion used for cloning the Order.
    public abstract Order clone();

    // Abstract method to check if the order is possible.
    public abstract boolean isPossible(ShareSimulation shareSimulation);

    // Getter for retrieving the share symbol.
    public String getShare() {
        return share;
    }

    // Getter for retrieving the number of shares.
    public int getNumberOfShares() {
        return numberOfShares;
    }

    // Getter for retrieving the price per share.
    public int getPrice() {
        return price;
    }

    // Getter for retrieving the investor.
    public Investor getInvestor() {
        return investor;
    }

    // Getter for retrieving the turn.
    public int getTurn() {
        return turn;
    }

    // Getter for retrieving the position within the turn.
    public int getWhichInTurn() {
        return whichInTurn;
    }

    // Getter for retrieving the type of the order.
    public boolean getType() {
        return type;
    }

    // Setter for updating the number of shares.
    public void setNumberOfShares(int numberOfShares) {
        this.numberOfShares = numberOfShares;
    }
    public boolean isPossibleWA(ShareSimulation shareSimulation, int purchaseIndex, int saleIndex, boolean side, Queue sale, Queue purchase) {
        return true;
    }

    public void setInvestor(Investor investor) {
        this.investor = investor;
    }
}