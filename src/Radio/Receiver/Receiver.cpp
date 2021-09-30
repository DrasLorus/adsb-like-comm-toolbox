#include "Receiver.hpp"

Receiver::Receiver(float s_fc, float s_fe){
    fc         = s_fc;
    fe         = s_fe;
    _alive     = true;

    //
    // On veut une seconde de signal pour ne rien rater...
    //
//    const uint32_t echanti = (2 * s_fe);
//    const uint32_t symbols = (echanti + 262144 - 1) / 262144;
}


Receiver::~Receiver()
{

}

bool Receiver::alive()
{
    return _alive;
}
