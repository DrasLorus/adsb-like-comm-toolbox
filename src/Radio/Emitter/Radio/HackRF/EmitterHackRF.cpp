#include "EmitterHackRF.hpp"
#include <unistd.h>

typedef enum {
    HW_SYNC_MODE_OFF = 0,
    HW_SYNC_MODE_ON = 1,
} hw_sync_mode_t;


#define FREQ_ONE_MHZ (1000000ll)

EmitterHackRF::EmitterHackRF(float s_fc, float s_fe)
    : Emitter(s_fc, s_fe), buff(4 * s_fe )
{
    freq_hz    =  s_fc;
    fech_hz    =  s_fe;
    amplifier  = false;
    antenna    =  true;
    txvga_gain =    32;
}


EmitterHackRF::EmitterHackRF(Parameters& param)
    : Emitter(param.toFloat("fc"), param.toFloat("fe")),
      buff( 4 * param.toInt("fe") )
{
    freq_hz    =  param.toFloat("fc");
    fech_hz    =  param.toFloat("fe");
    amplifier  = param.toFloat("amplifier");
    antenna    = param.toFloat("antenna");
    txvga_gain = param.toFloat("tx_gain");
}


EmitterHackRF::~EmitterHackRF()
{
    hackrf_close( device );
//    delete[] buffer;
}


int EmitterHackRF::tx_callback(hackrf_transfer* transfer)
{
    EmitterHackRF *obj = (EmitterHackRF*) transfer->tx_ctx;
    return obj->tx_callback(transfer->buffer, transfer->valid_length);
}


int EmitterHackRF::tx_callback(unsigned char *buf, uint32_t len)
{
    const std::lock_guard<std::mutex> lock(g_i_mutex);
    const uint32_t to_send = (buff.NumElements() >= len) ? buff.NumElements() : len;
    const uint32_t sended  = buff.Read((int8_t*)buf, to_send);
    memset(buf + sended, 0, len - sended);
    return 0;
}


void EmitterHackRF::initialize()
{
    std::vector<std::string> modules;
    modules.push_back("0000000000000000088869dc334b441b");
    modules.push_back("000000000000000075b068dc317bae07");
    modules.push_back("000000000000000026b468dc33776d8f");
    modules.push_back("0000000000000000088869dc242e9d1b");
    modules.push_back("0000000000000000088869dc3362561b");

    int result;
    result = hackrf_init();
    if( result != HACKRF_SUCCESS ) {
        fprintf(stderr, "hackrf_init() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }

    for(uint32_t n = 0; n < modules.size(); n += 1)
    {
        result = hackrf_open_by_serial(modules.at(n).c_str(), &device);
        if( result == HACKRF_SUCCESS ) break;
    }

    if( result != HACKRF_SUCCESS ) {
        fprintf(stderr, "hackrf_open() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }

    fprintf(stderr, "call hackrf_set_hw_sync_mode(%d)\n", 0);
    result = hackrf_set_hw_sync_mode(device, 0 ? HW_SYNC_MODE_ON : HW_SYNC_MODE_OFF);
    if( result != HACKRF_SUCCESS ) {
        fprintf(stderr, "hackrf_set_hw_sync_mode() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }

    set_freq          ( freq_hz );
    set_sample_rate   ( fech_hz );
    set_amp_enable    (   false );
    set_antenna_enable(   true  );
    set_txvga_gain    (      32 );
}


void EmitterHackRF::set_freq(double value)
{
    freq_hz = value;
    fprintf(stderr, "call hackrf_set_freq(%f Hz/%.03f MHz)\n", freq_hz, ((double)freq_hz/(double)FREQ_ONE_MHZ) );
    int32_t result = hackrf_set_freq(device, freq_hz);
    if( result != HACKRF_SUCCESS ) {
        fprintf(stderr, "hackrf_set_freq() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }
}


double EmitterHackRF::get_freq( )
{
    return freq_hz;
}


void EmitterHackRF::set_sample_rate(double value)
{
    fech_hz = value;
    fprintf(stderr, "call hackrf_set_sample_rate(%f Hz/%.03f MHz)\n", fech_hz, ((double)fech_hz/(double)FREQ_ONE_MHZ) );
    int32_t result = hackrf_set_sample_rate(device, fech_hz);
    if( result != HACKRF_SUCCESS ) {
        fprintf(stderr, "hackrf_set_sample_rate() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }    
}


double EmitterHackRF::get_sample_rate()
{
    return fech_hz;
}


void EmitterHackRF::set_amp_enable(bool value)
{
    fprintf(stderr, "call hackrf_set_amp_enable(%u => %u)\n", amplifier, value);
    amplifier = value;
    const uint8_t control = amplifier ? 14 : 0;                         // Pour activer l'amplificateur la valeur a fournir
    int32_t result = hackrf_set_amp_enable(device, (uint8_t)control);   // doit etre 14, 0 sinon...
    if( result != HACKRF_SUCCESS ) {
        fprintf(stderr, "hackrf_set_amp_enable() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }
}


bool EmitterHackRF::get_amp_enable( )
{
    return amplifier;
}


void EmitterHackRF::set_antenna_enable(bool value)
{
    fprintf(stderr, "call hackrf_set_antenna_enable(%u => %u)\n", antenna, value);
    antenna = value;
    int32_t result = hackrf_set_antenna_enable(device, (uint8_t)antenna);
    if (result != HACKRF_SUCCESS) {
        fprintf(stderr, "hackrf_set_antenna_enable() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }
}


bool EmitterHackRF::get_antenna_enable( )
{
    return antenna;
}


void EmitterHackRF::inc_txvga_gain()
{
    const int32_t  value = get_txvga_gain() + 1;
    const int32_t nvalue = (value > 47) ? 47 : value;
    set_txvga_gain( nvalue );
}


void EmitterHackRF::dec_txvga_gain()
{
    const int32_t  value = get_txvga_gain() - 1;
    const int32_t nvalue = (value < 0) ? 0 : value;
    set_txvga_gain( nvalue );
}


void EmitterHackRF::set_txvga_gain(uint32_t value)
{
    fprintf(stderr, "call hackrf_set_txvga_gain(%u => %u)\n", txvga_gain, value);
    assert( value   >=  0 );
    assert( value   <= 47 );

    txvga_gain = value;
    int32_t result  = hackrf_set_txvga_gain(device, txvga_gain);
    if( result != HACKRF_SUCCESS ) {
        fprintf(stderr, "hackrf_set_txvga_gain() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }
}


uint32_t EmitterHackRF::get_txvga_gain( )
{
    return txvga_gain;
}


void EmitterHackRF::start_engine( )
{
    fprintf(stderr, "(II) Emitter::start_engine() is executed...\n");
    const std::lock_guard<std::mutex> lock(g_i_mutex);

    const int result = hackrf_start_tx(device, tx_callback, (void *)this);
    if( result != HACKRF_SUCCESS ) {
        fprintf(stderr, "hackrf_start_rx() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }
}


void EmitterHackRF::stop_engine( )
{
    fprintf(stderr, "(II) Emitter::stop_engine() is executed...\n");
    while(buff.IsData() == true)
    {
        usleep(1000); // queue empty
    }

    const int result = hackrf_stop_tx(device);
    if( result != HACKRF_SUCCESS )
    {
        fprintf(stderr, "hackrf_stop_tx() failed: %s (%d)\n", hackrf_error_name((hackrf_error)result), result);
        exit( EXIT_FAILURE );
    }
}


void EmitterHackRF::emission(std::vector<int8_t>& cbuffer )
{
    if( hackrf_is_streaming(device) != HACKRF_TRUE )
    {
        printf("(EE) The hackrf device is currently stopped, it is impossible to transfer data...\n");
        exit( EXIT_FAILURE );
    }

    const uint32_t byte_to_send = cbuffer.size();
    while( buff.NumFreeElements() < byte_to_send )
    {
        usleep(1000); // queue empty
    }

    const uint32_t nWrite = buff.Write(cbuffer.data(), byte_to_send);
    if( nWrite != byte_to_send )
    {
        printf("(EE) We got an issue when the data set was loaded in the buffer...\n");
        exit( EXIT_FAILURE );
    }
    const std::lock_guard<std::mutex> lock(g_i_mutex);

    while( buff.NumFreeElements() < byte_to_send )
    {
        usleep(1000); // queue empty
    }

    memset( cbuffer.data(), 0, byte_to_send );
    const uint32_t nWritE = buff.Write(cbuffer.data(), byte_to_send);
    if( nWritE != byte_to_send )
    {
        printf("(EE) We got an issue when the data set was loaded in the buffer...\n");
        exit( EXIT_FAILURE );
    }
}


void EmitterHackRF::close()
{

}


void EmitterHackRF::reset()
{
    fprintf(stderr, "Emitter::reset() not implemented yet !\n");
    exit( EXIT_FAILURE );
}
