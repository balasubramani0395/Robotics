#include "matchEngine.h"

int main() {
  std::cerr << "====== Match Engine =====" << std::endl;
  std::cerr << "Enter 'exit' to quit" << std::endl;
  TradeVector tdVector;
  std::string line;

  while (getline(std::cin, line) && line != "exit") {
    std::cout << "Received: '" << line << "'" << std::endl;
    Trade t1;
    std::stringstream s(line);
    s >> t1.OrderID >> t1.Side >> t1.Instrument >> t1.Quantity >> t1.Price;
    tdVector.tradeVector.push_back(t1);
  }
  tdVector.matchTrade();
  tdVector.printUnmatchedTrade();

  return 0;
}
