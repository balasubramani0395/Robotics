
#include <iostream>
using namespace std;

class Color{
private:
    uint8_t r;
    uint8_t g;
    uint8_t b;

public:
    Color(uint8_t R,uint8_t G,uint8_t B);
    void getColor(uint8_t &R,uint8_t &G,uint8_t &B);;
    void getColor(double &rd, double &gd, double &bd);;
    uint32_t getColor();;
};