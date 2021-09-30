#ifndef _ReceiverFileBZ2_
#define _ReceiverFileBZ2_

#include "../Receiver.hpp"
#include <bzlib.h>

class ReceiverFileBZ2 : public Receiver{
private:
    vector<uint8_t> buffer;
    FILE*   stream;
    BZFILE* streaz;

    bool unsigned_mode;

public :
    ReceiverFileBZ2(std::string filen, const bool _unsigned_ = false);
	~ReceiverFileBZ2();

    void initialize();
    bool reception(vector<complex<float> >& cbuffer, const uint32_t coverage = 0);
    void reset();

    void start_engine();
    void stop_engine ();
};

#endif
