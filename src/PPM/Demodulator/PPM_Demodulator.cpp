#include "PPM_Demodulator.hpp"


PPM_Demodulator::PPM_Demodulator()
{

}


PPM_Demodulator::~PPM_Demodulator()
{
	
}


void PPM_Demodulator::execute(std::vector<float>& ibuffer, std::vector<uint8_t>& obuffer)
{
	// Le buffer de sortie doit etre 2x plus petit...
    if( obuffer.size() != (ibuffer.size()/2) )
    {
        obuffer.resize(ibuffer.size()/2);
    }

    const uint32_t ll = obuffer.size();
    for(uint32_t i = 0 ; i < ll; i += 1)
    {
        const float left  = ibuffer[2*i  ];
        const float right = ibuffer[2*i+1];
        obuffer[i] = (left > right);
    }
}


void PPM_Demodulator::execute(std::vector<int8_t>& ibuffer, std::vector<uint8_t>& obuffer)
{
    // Le buffer de sortie doit etre 2x plus petit...
    if( obuffer.size() != (ibuffer.size()/2) )
    {
        obuffer.resize(ibuffer.size()/2);
    }

    const uint32_t ll = obuffer.size();
    for(uint32_t i = 0 ; i < ll; i += 1)
    {
        const int8_t left  = ibuffer[2*i  ];
        const int8_t right = ibuffer[2*i+1];
        obuffer[i] = (left > right);
    }
}