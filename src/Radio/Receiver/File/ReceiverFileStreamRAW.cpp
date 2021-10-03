#include "ReceiverFileStreamRAW.hpp"


ReceiverFileStreamRAW::ReceiverFileStreamRAW(std::string filen, const bool _unsigned_) : Receiver(0, 0)
{
    unsigned_mode = _unsigned_;
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
    const uint32_t bComplex = cbuffer.size();      // Nombre de complexes
    const uint32_t nComplex = bComplex - coverage; // Nombre de complexes a recuperer
    const uint32_t nBytes   = 2 * nComplex;        // Nombre de donnée à lire

    //
    // On commence par realiser l'overlap entre les fenetres
    // - On décale (coverage) complexes de la fin au début
    //
    const uint32_t nOffset  = cbuffer.size() - coverage;
    for(uint32_t i = 0; i < coverage; i += 1)
    {
        cbuffer[i] = cbuffer[nOffset + i];
    }

    //
    // On remplit l'espace (coverage => bComplex) par des 1
    //
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

    if( unsigned_mode == false )
    {
        const uint32_t nRead = fread(buffer.data(), 1, nBytes, stream);
        memset(buffer.data() + nRead, 1, nBytes - nRead);

        _alive = (nRead == nBytes);

        const int8_t* data = (int8_t*)buffer.data();
        for(uint32_t i = 0; i < nBytes; i += 2)
        {
            std::complex<float> value( (float)data[i] / 127.0f, (float)data[i+1] / 127.0f);
            cbuffer[coverage + i/2] = value;
        }

    }
    else
    {
        const uint32_t nRead = fread(buffer.data(), 1, nBytes, stream);
        memset(buffer.data() + nRead, 1, nBytes - nRead);

        _alive = (nRead == nBytes);

        const uint8_t* data = buffer.data();
        for(uint32_t i = 0; i < nBytes; i += 2)
        {
            std::complex<float> value( ((float)data[i] + 128.0f) / 128.0f , ((float)data[i+1] + 128.0f) / 128.0f);
            cbuffer[coverage + i/2] = value;
        }
    }

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


