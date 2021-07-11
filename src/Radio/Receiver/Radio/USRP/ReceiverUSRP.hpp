#ifndef _RadioUSRP_
#define _RadioUSRP_

#include "../../Receiver.hpp"
#include "../../../../couleur.h"

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd.h>

class ReceiverUSRP : public Receiver{
private :
	uhd::usrp::multi_usrp::sptr usrp;              // Le pointeur vers l'USRP
    uhd::rx_streamer::sptr rx_stream;

public :
    ReceiverUSRP(float s_fc, float s_fe);
	~ReceiverUSRP();
    void initialize();
    bool reception(vector<complex<float> >& buffer, const uint32_t coverage = 0);

    virtual void reset();
    virtual void start_engine();
    virtual void stop_engine ();

    void set_rx_gain(const float gain_value);

};

#endif