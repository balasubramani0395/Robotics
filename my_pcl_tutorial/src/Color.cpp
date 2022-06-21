
#include <my_pcl_tutorial/Color.hpp>

Color::Color(uint8_t R,uint8_t G,uint8_t B):r(R),g(G),b(B){}

void Color::getColor(uint8_t &R,uint8_t &G,uint8_t &B){
    R = r;
    G = g;
    B = b;
}
void Color::getColor(double &rd, double &gd, double &bd){
    rd = (double)r/255;
    gd = (double)g/255;
    bd = (double)b/255;
}
uint32_t Color::getColor(){
    return ((uint32_t)r<<16|(uint32_t)g<<8|(uint32_t)b);
}
