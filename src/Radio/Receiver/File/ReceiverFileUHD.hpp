#ifndef _RadioFichierUHD_
#define _RadioFichierUHD_

#include "../Receiver.hpp"


class ReceiverFileUHD : public Receiver{
private :
    std::vector< std::complex<float> > data;
    std::string filename;

public :
    ReceiverFileUHD(std::string filen);
	~ReceiverFileUHD();

    void initialize();
    bool reception(vector<complex<float> >& cbuffer, const uint32_t coverage = 0);

    void reset();

    virtual void start_engine();
    virtual void stop_engine ();

};

#endif