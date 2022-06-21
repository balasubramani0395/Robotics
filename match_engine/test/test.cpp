#include "test.h"

TradeVector tdVectorTest ;
TradeVector tdVector ;

TEST(MatchEngineTest, MatchedTrades) {
  // Compare the Expected Output vector size and Actual Output vector size
  EXPECT_EQ(tdVectorTest.tradeVectorOutput.size(),
            tdVector.tradeVectorOutput.size());
  if (tdVectorTest.tradeVectorOutput.size() ==
      tdVector.tradeVectorOutput.size()) {
    for (int i = 0; i < tdVector.tradeVectorOutput.size(); i++) {
      // Compare All parameters in the Output trade details which is matched
      // with other trades
      EXPECT_EQ(tdVectorTest.tradeVectorOutput[i].code,
                tdVector.tradeVectorOutput[i].code);
      EXPECT_EQ(tdVectorTest.tradeVectorOutput[i].ContraOrderID,
                tdVector.tradeVectorOutput[i].ContraOrderID);
      EXPECT_EQ(tdVectorTest.tradeVectorOutput[i].OrderID,
                tdVector.tradeVectorOutput[i].OrderID);
      EXPECT_EQ(tdVectorTest.tradeVectorOutput[i].Price,
                tdVector.tradeVectorOutput[i].Price);
      EXPECT_EQ(tdVectorTest.tradeVectorOutput[i].Quantity,
                tdVector.tradeVectorOutput[i].Quantity);
    }
  }
}

// Compare All parameters in the Output trade details which is unmatched with
// other trades
TEST(MatchEngineTest, UnmatchedTrades) {
  int j = 0;
  for (int i = 0; i < tdVector.tradeVector.size(); i++) {
    if (tdVector.tradeVector[i].Quantity != 0) {
      EXPECT_EQ(tdVectorTest.tradeVector[j].Side, tdVector.tradeVector[i].Side);
      EXPECT_EQ(tdVectorTest.tradeVector[j].OrderID,
                tdVector.tradeVector[i].OrderID);
      EXPECT_EQ(tdVectorTest.tradeVector[j].Instrument,
                tdVector.tradeVector[i].Instrument);
      EXPECT_EQ(tdVectorTest.tradeVector[j].Price,
                tdVector.tradeVector[i].Price);
      EXPECT_EQ(tdVectorTest.tradeVector[j].Quantity,
                tdVector.tradeVector[i].Quantity);
      j += 1;
    }
  }
}
int main(int argc, char **argv) {
  // Read expected unmatched trade file and store it in a vector
  std::string fileName = "/app/testFiles/unmatched.txt";
  std::ifstream myfile(fileName.c_str());
  std::string line;

  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      Trade t1;
      std::stringstream s(line);
      s >> t1.OrderID >> t1.Side >> t1.Instrument >> t1.Quantity >> t1.Price;
      tdVectorTest.tradeVector.push_back(t1);
    }
  }
  // Read expected matched trade file and store it in a vector
  std::string fileName2 = "/app/testFiles/output.txt";
  std::ifstream myfile2(fileName2.c_str());

  if (myfile2.is_open()) {
    while (getline(myfile2, line)) {
      TradeOutput t2;
      std::stringstream s(line);
      s >> t2.code >> t2.Instrument >> t2.OrderID >> t2.ContraOrderID >>
          t2.Quantity >> t2.Price;
      tdVectorTest.tradeVectorOutput.push_back(t2);
    }
  }

  // Read input trade file and store it in a vector
  std::string fileName3 = "/app/inputFiles/sample_input.txt";
  std::ifstream myfile3(fileName3.c_str());

  if (myfile3.is_open()) {
    while (getline(myfile3, line)) {
      Trade t3;
      std::stringstream s(line);
      s >> t3.OrderID >> t3.Side >> t3.Instrument >> t3.Quantity >> t3.Price;
      tdVector.tradeVector.push_back(t3);
    }
  }
  // Call matchTrade function to match compatible trades for testing
  tdVector.matchTrade();

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}