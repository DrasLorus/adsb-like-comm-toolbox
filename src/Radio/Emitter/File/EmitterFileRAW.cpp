#include "EmitterFileRAW.hpp"


EmitterFileRAW::EmitterFileRAW(std::string filen, const bool emptySlots) : Emitter(0, 0)
{
    filename   = filen;
    stream     = nullptr;
    firstFrame = true;
    this->emptySlots = emptySlots;

}


EmitterFileRAW::EmitterFileRAW(Parameters& param, bool emptySlots) : Emitter(0, 0)
{
    filename   = param.toString("filename");
    stream     = nullptr;
    firstFrame = true;
    this->emptySlots = emptySlots;
}


EmitterFileRAW::~EmitterFileRAW()
{
    if (stream != NULL){
        fclose( stream );
    }
}


void EmitterFileRAW::initialize()
{
    stream = fopen(filename.c_str(), "wb");

    if (stream == nullptr){
        fprintf(stderr, "ReceiverFileRAW::initialize() error during file openning (%s) !\n", filename.c_str());
        exit( EXIT_FAILURE );
    }
}


void EmitterFileRAW::start_engine()
{

}


void EmitterFileRAW::stop_engine ()
{

}


void EmitterFileRAW::close()
{
    // On ajoute quelques données dans le fichier
    int8_t buff[4096];
    for(uint32_t i = 0; i < 4096; i += 1)
    buff[i] = (rand()%8) - 4;
    fwrite(buff, 4096, sizeof(int8_t), stream);
    // On ajoute quelques données dans le fichier

    if (stream == nullptr){
        fprintf(stderr, "ReceiverFileRAW::close() error during file close operation (%s) !\n", filename.c_str());
        exit( EXIT_FAILURE );
    }
    if (stream != nullptr){
        fclose( stream );
        stream = nullptr;
    }
}

#define _STORE_ZERO_

void EmitterFileRAW::emission( std::vector<int8_t>& cbuffer )
{

    if( emptySlots == true || (firstFrame == true) )
    {
        const uint32_t length = cbuffer.size() / 32;
        int8_t* buff = new int8_t[length];
        for(uint32_t i = 0; i < length; i += 1)
            buff[i] = (rand()%8) - 4;
        fwrite(buff, length, sizeof(int8_t), stream);
        delete[]  buff;
    }

    fwrite(cbuffer.data(), cbuffer.size(), sizeof(int8_t), stream);

    if( emptySlots == true )
    {
        const uint32_t length = cbuffer.size() / 32;
        int8_t* buff = new int8_t[length];
        for(uint32_t i = 0; i < length; i += 1)
            buff[i] = (rand()%8) - 4;
        fwrite(buff, length, sizeof(int8_t), stream);
        delete[]  buff;
    }

    firstFrame = false;
}

// void EmitterFileRAW::set_txvga_gain(uint32_t value)
// {

// }


void EmitterFileRAW::reset()
{
    fprintf(stderr, "(EE) ReceiverFileRAW::reset() not implemented yet !\n");
    exit( EXIT_FAILURE );
}


