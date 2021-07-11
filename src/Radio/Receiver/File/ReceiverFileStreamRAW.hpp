#ifndef _ReceiverFileStreamRAW_
#define _ReceiverFileStreamRAW_

#include "../Receiver.hpp"

class ReceiverFileStreamRAW : public Receiver{
private:
    vector<uint8_t> buffer;
    FILE* stream;

public :
    ReceiverFileStreamRAW(std::string filen);
	~ReceiverFileStreamRAW();

    void initialize();
    bool reception(vector<complex<float> >& cbuffer, const uint32_t coverage = 0);
    void reset();

    void start_engine();
    void stop_engine ();
};

#endif
