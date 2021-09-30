#ifndef _Radio_
#define _Radio_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <complex>
#include <vector>
#include <cstring>
#include <cassert>
#include <chrono>

using namespace std;

class Receiver{

protected:
	float   fc;
	float   fe;
    bool    _alive;

public :
    Receiver(float s_fc, float s_fe);
	virtual ~Receiver();
    virtual void initialize() = 0;

    virtual bool reception(std::vector< std::complex<float> >& cbuffer, const uint32_t coverage) = 0;
//  virtual bool reception(std::vector< std::complex<int8_t> >& cbuffer, const uint32_t coverage) = 0;
//  virtual bool reception(std::vector< std::complex<int16_t> >& cbuffer, const uint32_t coverage) = 0;

    virtual void reset() = 0;
    virtual bool alive();

    virtual void start_engine() = 0;
    virtual void stop_engine () = 0;
};

#endif
