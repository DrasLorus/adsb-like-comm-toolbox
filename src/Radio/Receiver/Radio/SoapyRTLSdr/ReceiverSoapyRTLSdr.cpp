#include "ReceiverSoapyRTLSdr.hpp"
#include <unistd.h>

ReceiverSoapyRTLSdr::ReceiverSoapyRTLSdr(const float s_fc, const float s_fe) : Receiver(s_fc, s_fe)
{
    if( s_fc < 24000000 )
    {
        printf("[SoapyRTLSdr] The working frequency value is too small (%f < 24000000)\n", s_fc);
        exit( EXIT_FAILURE );
    }

    if( s_fc > 1764000000 )
    {
        printf("[SoapyRTLSdr] The working frequency value is too high (%f > 1764000000)\n", s_fc);
        exit( EXIT_FAILURE );
    }
#if 0
    SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
    SoapySDR::Kwargs::iterator it;
    if( results.size() == 0 )
    {
        printf("[SoapyRTLSdr] No device was detected by SoapySDR::Device::enumerate() call\n");
        exit( EXIT_FAILURE );
    }

    int idex = 256;
    for( int i = 0; i < results.size(); ++i)  // On prend toujours le permier de la liste
    {
        printf("Found device #%d:\n", i);
        for( it = results[i].begin(); it != results[i].end(); ++it)
        {
            printf("%s = %s\n", it->first.c_str(), it->second.c_str());
            if( (it->first == "driver") && (it->second == "rtlsdr") )
            {
                idex = idex < i ? idex : i;
            }
        }
        printf("\n");
    }
    if( idex == 256 )
    {
        printf("[SoapyRTLSdr] No RTL-SdR device located on host...\n");
        exit( EXIT_FAILURE );
    }
#endif
    //
    // On instancie un objet de type driver
    //
    sdr = SoapySDR::Device::make("driver=rtlsdr");
    if( sdr == NULL )
    {
        fprintf(stderr, "SoapySDR::Device::make failed\n");
        exit( EXIT_FAILURE );
    }

    //	2.1 antennas
    std::vector< std::string > str_list;	//string list
    str_list = sdr->listAntennas( SOAPY_SDR_RX, 0);
    printf("Rx antennas: ");
    for(uint32_t i = 0; i < str_list.size(); ++i)
        printf("%s,", str_list[i].c_str());
    printf("\n");

    //	2.2 gains
    str_list = sdr->listGains( SOAPY_SDR_RX, 0);
    printf("Rx Gains: ");
    for(uint32_t i = 0; i < str_list.size(); ++i)
        printf("%s, ", str_list[i].c_str());
    printf("\n");

    //	2.3. ranges(frequency ranges)
    SoapySDR::RangeList ranges = sdr->getFrequencyRange( SOAPY_SDR_RX, 0);
    printf("Rx freq ranges: ");
    for(uint32_t i = 0; i < ranges.size(); ++i)
        printf("[%g Hz -> %g Hz], ", ranges[i].minimum(), ranges[i].maximum());
    printf("\n");

    std::cout << "Current sample rate : " << get_sample_rate() << std::endl;
    set_sample_rate( s_fe );
    std::cout << "Current sample rate : " << get_freq() << std::endl;
    set_freq       ( s_fc );
    set_tuner_gain (0 );

    // 4. setup a stream (complex floats)
    rx_stream = sdr->setupStream( SOAPY_SDR_RX, SOAPY_SDR_CF32);
    if( rx_stream == NULL)
    {
        fprintf( stderr, "Failed\n");
        SoapySDR::Device::unmake( sdr );
        exit( EXIT_FAILURE );
    }
}

ReceiverSoapyRTLSdr::~ReceiverSoapyRTLSdr()
{
    sdr->closeStream( rx_stream );
    SoapySDR::Device::unmake( sdr );
}


void ReceiverSoapyRTLSdr::start_engine()
{
    printf("[ START ReceiverSoapyRTLSdr ]\n");
    sdr->activateStream( rx_stream, 0, 0, 0);
}


void ReceiverSoapyRTLSdr::stop_engine()
{
    printf("[ STOP ReceiverSoapyRTLSdr ]\n");
    sdr->deactivateStream( rx_stream, 0, 0);	//stop streaming
}


void ReceiverSoapyRTLSdr::initialize()
{

}


void ReceiverSoapyRTLSdr::set_freq(const double value)
{
    try{
        sdr->setFrequency( SOAPY_SDR_RX, 0, value );
    }catch(exception& e)
    {

    }
    printf("[ FREQUENCY : %f ]\n", get_freq());
}


double ReceiverSoapyRTLSdr::get_freq( )
{
    return sdr->getFrequency( SOAPY_SDR_RX, 0 );
}


void ReceiverSoapyRTLSdr::set_sample_rate(const double value)
{
    sdr->setSampleRate( SOAPY_SDR_RX, 0, value );
    printf("[ SAMPLE RT : %f ]\n", get_sample_rate());
}


double ReceiverSoapyRTLSdr::get_sample_rate( )
{
    return sdr->getSampleRate( SOAPY_SDR_RX, 0 );
}


void ReceiverSoapyRTLSdr::set_tuner_gain(const double value)
{
    sdr->setGain(SOAPY_SDR_RX, 0, "TUNER",  value);
    printf("[ReceiverSoapyRTLSdr] set TUNER gain value : %lf\n", get_tuner_gain());
}


double ReceiverSoapyRTLSdr::get_tuner_gain( )
{
    return sdr->getGain(SOAPY_SDR_RX, 0, "TUNER");
}


bool ReceiverSoapyRTLSdr::reception( std::vector< std::complex<float> >& cbuffer, const uint32_t coverage)
{
    //
    // On gere le vieillissement du buffer d'echantillons !
    //
    const uint32_t nOffset  = cbuffer.size() - coverage;
    for(uint32_t loop = 0; loop < coverage; loop += 1)
    {
        cbuffer[loop] = cbuffer[nOffset + loop];
    }
    //
    // Fin de la gestion du vieillissement du buffer
    //

    uint32_t to_read = cbuffer.size();
    uint32_t nb_read = coverage;
    std::complex<float>* buff = cbuffer.data();

    int flags;
    long long time_ns;
    do{
        void *buffs[]  = {buff + nb_read};
        uint32_t nRead = sdr->readStream( rx_stream, buffs, to_read - nb_read, flags, time_ns, 1000000);
        if( nRead < 0 ) exit( EXIT_FAILURE );
        nb_read       += nRead;
        if( nb_read != to_read ){
            usleep( 500 );
        }
    }while( (to_read - nb_read) != 0 );

#if 1
    for(uint32_t i = 0; i < to_read; i += 1)
        buff[i] *= 127.0f;
#else
    float* pbuff = (float *)cbuffer.data();
    for(int i = 0; i < 2 * to_read; i += 1)
    {
        pbuff[i] = pbuff[i] * 127.0f;
    }
#endif
    return true;
}


void ReceiverSoapyRTLSdr::reset()
{
    fprintf(stderr, "ReceiverSoapyRTLSdr::reset() not implemented yet !\n");
    exit( EXIT_FAILURE );
}