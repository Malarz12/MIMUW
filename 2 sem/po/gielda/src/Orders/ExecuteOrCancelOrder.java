package Orders;

import Investors.Investor;
import Queue.*;
import Simulation.ShareSimulation;

public class ExecuteOrCancelOrder extends ImmediateOrder {
    public ExecuteOrCancelOrder(int numberOfShares, int price, Investor investor, String share, int turn, int whichInTurn, boolean type) {
        super(numberOfShares, price, investor, share, turn, whichInTurn, type);
    }

    @Override
    public boolean isPossible(ShareSimulation shareSimulation) {
        // Check if the turn of shareSimulation is greater than the last turn or number of shares is less than or equal to 0.
        if (shareSimulation.getTurn() > getLastTurn() || getNumberOfShares() <= 0) {
            // If the order is a sale and cannot be executed, return the shares to the investor's wallet.
            if (!getType()) {
                getInvestor().getWallet().put(getShare(), getInvestor().getWallet().get(getShare()) + getNumberOfShares());
            }
            return false;
        } else {
            return true;
        }
    }

    @Override
    public boolean isPossibleWA(ShareSimulation shareSimulation, int purchaseIndex, int saleIndex, boolean side, Queue sale, Queue purchase) {
        // Create temporary queues to simulate transactions.
        Queue tempSaleQueue = new SaleQueue(sale);
        Queue tempPurchaseQueue = new PurchaseQueue(purchase);
        for (int i = 0; i < sale.getQueue().size(); i++) {
            for (int j = 0; j < sale.getQueue().size(); j++) {
                if(i != j && sale.getQueue().get(i).getInvestor() == sale.getQueue().get(j).getInvestor()) {
                    tempSaleQueue.getQueue().get(j).setInvestor(tempSaleQueue.getQueue().get(i).getInvestor());
                }
            }
        }
        for (int i = 0; i < sale.getQueue().size(); i++) {
            for (int j = 0; j < purchase.getQueue().size(); j++) {
                if(sale.getQueue().get(i).getInvestor() == purchase.getQueue().get(j).getInvestor()) {
                    tempPurchaseQueue.getQueue().get(j).setInvestor(tempSaleQueue.getQueue().get(i).getInvestor());
                }
            }
        }

        if(side) { // If the order is a purchase.
            for(int i = saleIndex; i < tempSaleQueue.getQueue().size(); i++) {
                // If the number of shares in the purchase order is zero or less, the order can be executed.
                if (tempPurchaseQueue.get(purchaseIndex).getNumberOfShares() <= 0) {
                    return true;
                    // If the price of the sale order is greater than the purchase order, the order cannot be executed.
                } else if (tempSaleQueue.get(i).getPrice() > tempPurchaseQueue.get(purchaseIndex).getPrice()) {
                    return false;
                } else {
                    // Check if the sale order is an instance of ExecuteOrCancelOrder.
                    if (tempSaleQueue.get(i) instanceof ExecuteOrCancelOrder) {
                        // If the number of shares in the sale order is less than or equal to the purchase order, execute the transaction.
                        if (tempSaleQueue.get(i).getNumberOfShares() <= tempPurchaseQueue.get(purchaseIndex).getNumberOfShares()) {
                            shareSimulation.transaction(tempPurchaseQueue.get(purchaseIndex), tempSaleQueue.get(i), false);
                        } else if(tempSaleQueue.get(i).getNumberOfShares() > 0) {
                            int n = tempPurchaseQueue.get(purchaseIndex).getNumberOfShares();
                            tempPurchaseQueue.get(purchaseIndex).setNumberOfShares(0);
                            int m = tempSaleQueue.get(i).getNumberOfShares();
                            tempSaleQueue.get(i).setNumberOfShares(m - n);
                            // Recursively check if the order can be executed with the remaining shares.
                            if (isPossibleWA(shareSimulation, purchaseIndex, i, false, tempSaleQueue, tempPurchaseQueue)){
                                tempPurchaseQueue.get(purchaseIndex).setNumberOfShares(n);
                                tempSaleQueue.get(i).setNumberOfShares(m);
                                shareSimulation.transaction(tempPurchaseQueue.get(purchaseIndex), tempSaleQueue.get(i), false);
                            } else {
                                tempPurchaseQueue.get(purchaseIndex).setNumberOfShares(n);
                                tempSaleQueue.get(i).setNumberOfShares(m);
                            }
                        }
                    } else {
                        shareSimulation.transaction(tempPurchaseQueue.get(purchaseIndex), tempSaleQueue.get(i), false);
                    }
                }
            }
        } else { // If the order is a sale.
            for(int i = purchaseIndex; i < tempPurchaseQueue.getQueue().size(); i++) {
                // If the number of shares in the sale order is zero or less, the order can be executed.
                if (tempSaleQueue.get(saleIndex).getNumberOfShares() <= 0) {
                    return true;
                    // If the price of the purchase order is less than the sale order, the order cannot be executed.
                } else if (tempPurchaseQueue.get(i).getPrice() < tempSaleQueue.get(saleIndex).getPrice()) {
                    return false;
                } else {
                    // Check if the purchase order is an instance of ExecuteOrCancelOrder.
                    if (tempPurchaseQueue.get(i) instanceof ExecuteOrCancelOrder) {
                        // If the number of shares in the purchase order is less than or equal to the sale order, execute the transaction.
                        if (tempPurchaseQueue.get(i).getNumberOfShares() <= tempSaleQueue.get(saleIndex).getNumberOfShares()) {
                            shareSimulation.transaction(tempPurchaseQueue.get(i), tempSaleQueue.get(saleIndex), false);
                        } else if(tempPurchaseQueue.get(i).getNumberOfShares() > 0){
                            int n = tempSaleQueue.get(saleIndex).getNumberOfShares();
                            tempSaleQueue.get(saleIndex).setNumberOfShares(0);
                            int m = tempPurchaseQueue.get(i).getNumberOfShares();
                            tempPurchaseQueue.get(i).setNumberOfShares(m - n);
                            // Recursively check if the order can be executed with the remaining shares.
                            if (isPossibleWA(shareSimulation, i, saleIndex, true, tempSaleQueue, tempPurchaseQueue)){
                                tempPurchaseQueue.get(i).setNumberOfShares(m);
                                tempSaleQueue.get(saleIndex).setNumberOfShares(n);
                                shareSimulation.transaction(tempPurchaseQueue.get(i), tempSaleQueue.get(saleIndex), false);
                            } else {
                                tempPurchaseQueue.get(i).setNumberOfShares(m);
                                tempSaleQueue.get(saleIndex).setNumberOfShares(n);
                            }
                        }
                    } else {
                        shareSimulation.transaction(tempSaleQueue.get(saleIndex), tempPurchaseQueue.get(purchaseIndex), false);
                    }
                }
            }
        }
        return false;
    }

    @Override
    public Order clone() {
        // Create a clone of the investor.
        Investor temp = getInvestor().clone();
        // Return a new OpenEndedOrder with the same properties.
        Order result = new OpenEndedOrder(getNumberOfShares(), getPrice(), temp, getShare(), getTurn(), getWhichInTurn(), getType());
        return result;
    }
}