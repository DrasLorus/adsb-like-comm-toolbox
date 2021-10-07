#include "ReceiverThreadUSRP.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <string>
#include <mutex>

ReceiverThreadUSRP::ReceiverThreadUSRP(float s_fc, float s_fe) : Receiver(s_fc, s_fe), cthread(&ReceiverThreadUSRP::fetch_values, this)
{
    acq_ready = false;

    fc = s_fc;
    fe = s_fe;

    string usrp_addr("type=b200");                       // L'adresse de l'USRP est écrite en dur pour l'instant
    usrp = uhd::usrp::multi_usrp::make(usrp_addr); // Initialisation de l'USRP
}


ReceiverThreadUSRP::~ReceiverThreadUSRP()
{
    usrp.reset();
}


void ReceiverThreadUSRP::initialize(){
    usrp->set_clock_source("internal");
	usrp->set_rx_rate(fe);                         // Set de la fréquence d'échantillonnage
	usrp->set_rx_freq(fc);             // Set de la fréquence porteuse
	usrp->set_rx_antenna("RX2");

    uhd::stream_args_t stream_args("fc32", "sc16");
    stream_args.args["recv_buff_size"] = "1048576";

    // Type des données à échantillonner (ici complexes float 64 - 32 bits par voie)
	rx_stream = usrp->get_rx_stream(stream_args);    // Pointeur sur les data reçues

    cout << "[Thread-USRP] Clock source is set to        : internal]" << endl;
    cout << "[Thread-USRP] Sampling Rate set to         : " << usrp->get_rx_rate() << " MHz" << endl;
    cout << "[Thread-USRP] Central Frequency set to     : " << usrp->get_rx_freq() << " MHz" << endl;
    cout << "[Thread-USRP] Receiver gain is set to      : " << usrp->get_rx_gain() << " dB" << endl;
    cout << "[Thread-USRP] Reception antenna is set to  : RX2" << endl;
    cout << "[Thread-USRP] Sample data format is set to : fc32" << endl;
    cout << "[Thread-USRP] #channels for rx_stream      : " << rx_stream->get_num_channels() << " channel(s)" << endl;
    cout << "[Thread-USRP] #samples in rx_stream buffer : " << rx_stream->get_max_num_samps() << " IQs" << endl;
}


void ReceiverThreadUSRP::fetch_values()
{
    mutex_acq.lock();
    mutex_read.lock();

    while( true )
    {
        mutex_acq.lock();

        uhd::rx_metadata_t md; // Des metadata
        rx_stream->recv(ibuffer.data(), ibuffer.size(), md, 2.0); // 2 secondes de timeout...

        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
            std::cout << boost::format("Error a timeout appeared while streaming...") << std::endl;
            _alive = false;
            break;
        }

        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
            std::cerr << boost::format("Got an overflow indication.\n");
            _alive = false;
            break;
        }

        if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
            std::string error = str(boost::format("Receiver error: %s") % md.strerror());
            _alive = false;
            break;
        }

        mutex_read.unlock();
    }
}


bool ReceiverThreadUSRP::reception(vector<complex<float> >& buffer, const uint32_t coverage)
{
    const uint32_t nOffset  = buffer.size() - coverage;

    if( ibuffer.size() != nOffset )
        ibuffer.resize( nOffset );

    if( acq_ready == false ) {
        mutex_acq.unlock();
    } acq_ready = true;

    // Debut du vieillissement du buffer
    for(uint32_t loop = 0; loop < coverage; loop += 1)
    {
        buffer[loop] = buffer[nOffset + loop];
    }
    // Fin du vieillissement du buffer

    mutex_read.lock();

    // Debut de la copie des données
    for(uint32_t loop = 0; loop < nOffset; loop += 1)
    {
        buffer[coverage + loop] = ibuffer[loop];
    }
    // Fin de la copie des données

    mutex_acq.unlock();

    return _alive;
}


void ReceiverThreadUSRP::reset()
{
	usrp.reset();
}


void ReceiverThreadUSRP::start_engine()
{
    std::cout << "[ Thread-USRP engine begin  ]" << std::endl;
    usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
}


void ReceiverThreadUSRP::stop_engine()
{
    std::cout << "[ Thread-USRP engine end ]" << std::endl;
    usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
    rx_stream.reset();
}


void ReceiverThreadUSRP::set_rx_gain(const float gain_value)
{
    usrp->set_rx_gain(gain_value);
    std::cout << "[ Thread-USRP: Changing reception gain : " << usrp->get_rx_gain() << " dB ]" << std::endl;
}
