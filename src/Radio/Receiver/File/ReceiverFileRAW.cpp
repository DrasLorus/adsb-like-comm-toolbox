#include "ReceiverFileRAW.hpp"

ReceiverFileRAW::ReceiverFileRAW(std::string filen, const bool _unsigned_) : Receiver(0, 0)
{
    filename      = filen;
    unsigned_mode = _unsigned_;
}


ReceiverFileRAW::~ReceiverFileRAW()
{

}


void ReceiverFileRAW::initialize()
{
    auto start   = chrono::high_resolution_clock::now();

    FILE* stream = fopen(filename.c_str(), "rb");

    if (stream == NULL){
        fprintf(stderr, "ReceiverFileRAW::initialize() error during file openning (%s) !\n", filename.c_str());
        exit( EXIT_FAILURE );
    }

    fseek(stream, 0L, SEEK_END);        // On se place a la fin du fichier
    uint32_t fileSize = ftell(stream);  // On recupere la position actuelle
    fseek(stream, 0L, SEEK_SET);        // On se repositionne au debut du fichier

    data.resize( fileSize );
    fread(data.data(), 1, fileSize, stream);

    auto stop  = chrono::high_resolution_clock::now();
    auto timer = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
    std::cout << "(II) Time required to load the " << (fileSize/1024) << " kB is equal to " << timer << " ms" << std::endl;

    fclose(stream);
}


bool ReceiverFileRAW::reception(std::vector< std::complex<float> >& cbuffer, const uint32_t coverage)
{

    if( cbuffer.size() != (data.size()/2) ) // Nombre de symbols et non d'echantillons
    {
        cbuffer.resize(data.size()/2);
    }

    uint32_t length = data.size();
    if( unsigned_mode == false )
    {
        for(uint32_t i = 0; i < length; i += 2)
        {
            std::complex<float> value( (float)data[i], (float)data[i+1] );
            cbuffer[i/2] = value;
        }
    }else{
        const uint8_t* udata = (uint8_t*)data.data();
        for(uint32_t i = 0; i < length; i += 2)
        {
            const int32_t real = ((int32_t)udata[i  ]) - 128;
            const int32_t imag = ((int32_t)udata[i+1]) - 128;
            std::complex<float> value( real, imag );
            cbuffer[i/2] = value;
        }
    }
    _alive = false; // On stop le prog
    return true;
}


void ReceiverFileRAW::reset()
{
    fprintf(stderr, "RadioFichier::reset() not implemented yet !\n");
    exit( EXIT_FAILURE );
}


void ReceiverFileRAW::start_engine()
{

}


void ReceiverFileRAW::stop_engine()
{

}


