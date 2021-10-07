#ifndef _ThreadUSRP_
#define _ThreadUSRP_

#include "../../Receiver.hpp"
#include "../../../../couleur.h"

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd.h>
#include <thread>
#include <mutex>

class ReceiverThreadUSRP : public Receiver{
private :
	uhd::usrp::multi_usrp::sptr usrp;              // Le pointeur vers l'USRP
    uhd::rx_streamer::sptr rx_stream;

    vector<complex<float> > ibuffer;

    std::mutex mutex_acq;
    std::mutex mutex_read;
    std::thread cthread; // t1 is not a thread
    bool       acq_ready;

public :
    ReceiverThreadUSRP(float s_fc, float s_fe);
	~ReceiverThreadUSRP();
    void initialize();
    bool reception(vector<complex<float> >& buffer, const uint32_t coverage = 0);

    virtual void reset();
    virtual void start_engine();
    virtual void stop_engine ();

    void set_rx_gain(const float gain_value);

private:
    virtual void fetch_values();

};

#endif