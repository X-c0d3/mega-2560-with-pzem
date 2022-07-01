
#ifndef Pzem_h
#define Pzem_h
#include "Arduino.h"
class Pzem {
   public:
    float voltage;
    float current;
    float power;
    float energy;
    float frequency;
    float pf;
    float enegy_kw_day;
};

#endif