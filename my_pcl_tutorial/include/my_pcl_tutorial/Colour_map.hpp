
#include <iostream>

using namespace std;


class ColorMap{
public:
    ColorMap(double mn, double mx);
    void setMinMax(double min, double max);
    void setMin(double min);
    void setMax(double max);
    void getColor(double c,uint8_t& R, uint8_t& G, uint8_t& B);
    void getColor(double c, double &rd, double &gd, double &bd);
    
    uint32_t getColor(double c);

private:
    double interpolate(double val, double y0, double x0, double y1, double x1);

    double base(double val);
    double mn,mx;
};