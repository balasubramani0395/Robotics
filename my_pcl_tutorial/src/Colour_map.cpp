#include <my_pcl_tutorial/Colour_map.hpp>


ColorMap::ColorMap(double mn, double mx): mn(mn), mx(mx)
{

}
void ColorMap::setMinMax(double min, double max){ mn = min; mx = max;}
void ColorMap::setMin(double min){mn = min;}
void ColorMap::setMax(double max){mx = max;}
void ColorMap::getColor(double c,uint8_t& R, uint8_t& G, uint8_t& B){
    double normalized = (c - mn)/(mx-mn) * 2 - 1;
    R = (int) (base(normalized - 0.5) * 255);
    G = (int) (base(normalized) * 255);
    B = (int) (base(normalized + 0.5) * 255);
}
void ColorMap::getColor(double c, double &rd, double &gd, double &bd){
    uint8_t r;
    uint8_t g;
    uint8_t b;
    getColor(c,r,g,b);
    rd = (double)r/255;
    gd = (double)g/255;
    bd = (double)b/255;
}
uint32_t ColorMap::getColor(double c){
    uint8_t r;
    uint8_t g;
    uint8_t b;
    getColor(c,r,g,b);
    return ((uint32_t)r<<16|(uint32_t)g<<8|(uint32_t)b);
}



double ColorMap::interpolate(double val, double y0, double x0, double y1, double x1){
    return (val - x0)*(y1-y0)/(x1-x0) + y0;
}
double ColorMap::base(double val){
    if (val <= -0.75) return 0;
    else if (val <= -0.25) return interpolate(val,0,-0.75,1,-0.25);
    else if (val <= 0.25) return 1;
    else if (val <= 0.75) return interpolate(val,1.0,0.25,0.0,0.75);
    else return 0;
}

double mn,mx;
