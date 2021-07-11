#include "ReceiverFileStreamRAW.hpp"


ReceiverFileStreamRAW::ReceiverFileStreamRAW(std::string filen) : Receiver(0, 0)
{
    stream = fopen(filen.c_str(), "rb");
    if (stream == NULL){
        fprintf(stderr, "ReceiverFileStreamRAW::initialize() error during file openning (%s) !\n", filen.c_str());
        exit( EXIT_FAILURE );
    }
}


ReceiverFileStreamRAW::~ReceiverFileStreamRAW()
{
    fclose(stream);
}


void ReceiverFileStreamRAW::initialize()
{

}


bool ReceiverFileStreamRAW::reception(std::vector< std::complex<float> >& cbuffer, const uint32_t coverage)
{
    printf("ReceiverFileStreamRAW::reception() : cbuffer.size() = %lu | coverage = %u\n", cbuffer.size(), coverage);

    const uint32_t bComplex = cbuffer.size();
    const uint32_t nComplex = bComplex - coverage;
    const uint32_t nBytes   = 2 * nComplex;

    //
    // On commence par realiser l'overlap entre les fenetres
    //
    const uint32_t nOffset  = cbuffer.size() - coverage;
    for(uint32_t i = 0; i < coverage; i += 1)
    {
        cbuffer[i] = cbuffer[nOffset + i];
    }
    for(uint32_t i = coverage; i < bComplex; i += 1)
    {
        cbuffer[i] = 0;
    }

    //
    // On redimentionne le buffer interne à la taille du buffer que l'on doit fournir
    //
    if( buffer.size() != nBytes )
    {
        buffer.resize( nBytes ); // 2 fois plus d'échantillons car I/Q
    }

    //
    //
    //
    memset(buffer.data(), 1, nBytes);
    uint32_t nRead = fread(buffer.data(), 1, nBytes, stream);

    _alive = (nRead == nBytes);

    const uint8_t* data = buffer.data();
#if 1
    for(uint32_t i = 0; i < nBytes; i += 2)
    {
        std::complex<float> value( (float)data[i], (float)data[i+1] );
        cbuffer[coverage + i/2] = value;
    }
#else
    for(uint32_t i = 0; i < nRead; i += 2)
    {
        std::complex<float> value( (float)data[i], (float)data[i+1] );
        cbuffer[coverage + i/2] = value;
    }

    std::complex<float> cstv( 1, 1 );
    for(uint32_t i = coverage+(nRead/2); i < bComplex; i += 1)
    {
        cbuffer[coverage + i/2] = cstv;
    }
#endif
    printf("ReceiverFileStreamRAW::reception() : nRead  = %u\n", nRead);
    return true;
}


void ReceiverFileStreamRAW::reset()
{
    fprintf(stderr, "RadioFichier::reset() not implemented yet !\n");
    exit( EXIT_FAILURE );
}


void ReceiverFileStreamRAW::start_engine()
{

}


void ReceiverFileStreamRAW::stop_engine()
{

}


