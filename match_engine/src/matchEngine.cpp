#include "matchEngine.h"
#include <string>

// Match all the input buy and sell trades
void TradeVector::matchTrade() {
  std::string outputStr = "";
  TradeOutput tradeOut;
  for (int i = 0; i < tradeVector.size(); i++) {
    for (int j = 1; j < tradeVector.size(); j++) {
      // Check Instrument Match
      if (tradeVector[i].Instrument == tradeVector[j].Instrument &&
          tradeVector[i].Quantity != 0 && tradeVector[j].Quantity != 0) {
        // Check If Side is different
        if (tradeVector[i].Side != tradeVector[j].Side) {
          // Check If It is BUY or SELL
          if (tradeVector[i].Side == "BUY") {
            // Check whether BUY price is higher than SELL price
            if (tradeVector[i].Price >= tradeVector[j].Price) {
              // Filter lesser quantity and subtract from both trades
              if (tradeVector[i].Quantity < tradeVector[j].Quantity) {
                tradeOut.code = "TRADE";
                tradeOut.Instrument = tradeVector[j].Instrument;
                tradeOut.OrderID = tradeVector[j].OrderID;
                tradeOut.ContraOrderID = tradeVector[i].OrderID;
                tradeOut.Quantity = tradeVector[i].Quantity;
                tradeOut.Price = tradeVector[i].Price;
                tradeVectorOutput.push_back(tradeOut);
                tradeVector[j].Quantity =
                    tradeVector[j].Quantity - tradeVector[i].Quantity;
                tradeVector[i].Quantity =
                    tradeVector[i].Quantity - tradeVector[i].Quantity;

              } else {
                tradeOut.code = "TRADE";
                tradeOut.Instrument = tradeVector[j].Instrument;
                tradeOut.OrderID = tradeVector[j].OrderID;
                tradeOut.ContraOrderID = tradeVector[i].OrderID;
                tradeOut.Quantity = tradeVector[j].Quantity;
                tradeOut.Price = tradeVector[i].Price;
                tradeVectorOutput.push_back(tradeOut);
                tradeVector[i].Quantity =
                    tradeVector[i].Quantity - tradeVector[j].Quantity;
                tradeVector[j].Quantity =
                    tradeVector[j].Quantity - tradeVector[j].Quantity;
              }
            }
          } else {
            // Check whether BUY price is higher than SELL price
            if (tradeVector[i].Price <= tradeVector[j].Price) {
              // Filter lesser quantity and subtract from both trades
              if (tradeVector[i].Quantity < tradeVector[j].Quantity) {
                tradeOut.code = "TRADE";
                tradeOut.Instrument = tradeVector[j].Instrument;
                tradeOut.OrderID = tradeVector[i].OrderID;
                tradeOut.ContraOrderID = tradeVector[j].OrderID;
                tradeOut.Quantity = tradeVector[i].Quantity;
                tradeOut.Price = tradeVector[j].Price;
                tradeVectorOutput.push_back(tradeOut);
                tradeVector[j].Quantity =
                    tradeVector[j].Quantity - tradeVector[i].Quantity;
                tradeVector[i].Quantity =
                    tradeVector[i].Quantity - tradeVector[i].Quantity;
              } else {
                tradeOut.code = "TRADE";
                tradeOut.Instrument = tradeVector[j].Instrument;
                tradeOut.OrderID = tradeVector[i].OrderID;
                tradeOut.ContraOrderID = tradeVector[j].OrderID;
                tradeOut.Quantity = tradeVector[j].Quantity;
                tradeOut.Price = tradeVector[j].Price;
                tradeVectorOutput.push_back(tradeOut);
                tradeVector[i].Quantity =
                    tradeVector[i].Quantity - tradeVector[j].Quantity;
                tradeVector[j].Quantity =
                    tradeVector[j].Quantity - tradeVector[j].Quantity;
              }
            }
          }
        }
      }
    }
  }
}

// Print all matched and unmatched trades from the input trade
void TradeVector::printUnmatchedTrade() {
  for (int i = 0; i < tradeVectorOutput.size(); i++) {

    std::cout << tradeVectorOutput[i].code << " "
              << tradeVectorOutput[i].Instrument << " "
              << tradeVectorOutput[i].OrderID << " "
              << tradeVectorOutput[i].ContraOrderID << " "
              << tradeVectorOutput[i].Quantity << " "
              << tradeVectorOutput[i].Price << std::endl;
  }
  std::cout << std::endl;
  for (int i = 0; i < tradeVector.size(); i++) {
    if (tradeVector[i].Quantity != 0) {
      std::cout << tradeVector[i].OrderID << " " << tradeVector[i].Side << " "
                << tradeVector[i].Instrument << " " << tradeVector[i].Quantity
                << " " << tradeVector[i].Price << std::endl;
    }
  }
}