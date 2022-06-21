#ifndef MATCHENGINE_H
#define MATCHENGINE_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

struct Trade {
  std::string OrderID;
  std::string Side;
  std::string Instrument;
  int Quantity;
  double Price;
};

struct TradeOutput {
  std::string code;
  std::string OrderID;
  std::string ContraOrderID;
  std::string Instrument;
  int Quantity;
  double Price;
};

class TradeVector {
public:
  std::vector<Trade> tradeVector;
  std::vector<TradeOutput> tradeVectorOutput;

  void printUnmatchedTrade();
  void matchTrade();
};

#endif