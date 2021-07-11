#include "ReceiverUSRP.hpp"

ReceiverUSRP::ReceiverUSRP(float s_fc, float s_fe) : Receiver(s_fc, s_fe)
{
    fc = s_fc;
    fe = s_fe;

    string usrp_addr("type=b200");                       // L'adresse de l'USRP est écrite en dur pour l'instant
    usrp = uhd::usrp::multi_usrp::make(usrp_addr); // Initialisation de l'USRP
}

ReceiverUSRP::~ReceiverUSRP()
{
    usrp.reset();
}


void ReceiverUSRP::initialize(){
    usrp->set_clock_source("internal");
	usrp->set_rx_rate(fe);                         // Set de la fréquence d'échantillonnage
	usrp->set_rx_freq(fc);             // Set de la fréquence porteuse
	usrp->set_rx_antenna("RX2");

    uhd::stream_args_t stream_args("fc32", "sc16");  // Type des données à échantillonner (ici complexes float 64 - 32 bits par voie)
	rx_stream = usrp->get_rx_stream(stream_args);    // Pointeur sur les data reçues

    cout << "[UHD] Clock source is set to        : internal]" << endl;
    cout << "[UHD] Sampling Rate set to         : " << usrp->get_rx_rate() << " MHz" << endl;
    cout << "[UHD] Central Frequency set to     : " << usrp->get_rx_freq() << " MHz" << endl;
    cout << "[UHD] Receiver gain is set to      : " << usrp->get_rx_gain() << " dB" << endl;
    cout << "[UHD] Reception antenna is set to  : RX2" << endl;
    cout << "[UHD] Sample data format is set to : fc32" << endl;
    cout << "[UHD] #channels for rx_stream      : " << rx_stream->get_num_channels() << " channel(s)" << endl;
    cout << "[UHD] #samples in rx_stream buffer : " << rx_stream->get_max_num_samps() << " IQs" << endl;
}

//float vmin = +10000000;
//float vmax = -10000000;
//float vavg =         0;
//int cpt = 0;

bool ReceiverUSRP::reception(vector<complex<float> >& buffer, const uint32_t coverage)
{
//    bool overflow_message       = true;
//    bool continue_on_bad_packet = true;

#if 0
    if( cpt == 0 )
    {
        printf("vmin = %1.3f | vmax = %1.3f | vavg = %1.3f\r", vmin, vmax, vavg);
        fflush( stdout );
        vmin = +10000000;
        vmax = -10000000;
        vavg =         0;
        cpt = 0;
    }
    cpt = (cpt + 1) % 16;
#endif

    //
    // On gere le vieillissement du buffer d'echantillons !
    //
    const uint32_t nOffset  = buffer.size() - coverage;
    for(uint32_t loop = 0; loop < coverage; loop += 1)
    {
        buffer[loop] = buffer[nOffset + loop];
    }
    //
    // Fin de la gestion du vieillissement du buffer
    //

#if 0
    uhd::rx_metadata_t md;                     // Des metadata
    const uint32_t nSamples = buffer.size();
    uint32_t num_rx_samps = coverage; // Nombre d'echantillons liés au viellissement
    while (num_rx_samps < nSamples )
    {
        num_rx_samps += rx_stream->recv(&buffer.at(num_rx_samps), nSamples - num_rx_samps, md);
        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
            std::cout << boost::format("Error a timeout appeared while streaming...") << std::endl;
            exit( EXIT_FAILURE );
            break;
        }
        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
            if (overflow_message) {
                overflow_message = false;
                std::cerr
                        << boost::format(
                                "Got an overflow indication. Please consider the following:\n"
                                "  Your write medium must sustain a rate of %fMB/s.\n"
                                "  Dropped samples will not be written to the file.\n"
                                "  Please modify this example for your purposes.\n"
                                "  This message will not appear again.\n")
                           % (usrp->get_rx_rate(0) * sizeof(float) / 1e6);
            }
            continue;
        }
        if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
            std::string error = str(boost::format("Receiver error: %s") % md.strerror());
            if (continue_on_bad_packet) {
                std::cerr << error << std::endl;
                continue;
            } else
                throw std::runtime_error(error);
        }
    }
#else
    uhd::rx_metadata_t md; // Des metadata
    rx_stream->recv(&buffer.at(coverage), buffer.size() - coverage, md, 2.0); // 2 secondes de timeout...

    if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
        std::cout << boost::format("Error a timeout appeared while streaming...") << std::endl;
        _alive = false;
        return false; // On indique que l'aquisition s'est pas bien passée...
    }

    if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
            std::cerr
                        << boost::format(
                                "Got an overflow indication. Please consider the following:\n"
                                "  Your write medium must sustain a rate of %fMB/s.\n"
                                "  Dropped samples will not be written to the file.\n"
                                "  Please modify this example for your purposes.\n"
                                "  This message will not appear again.\n")
                           % (usrp->get_rx_rate(0) * sizeof(float) / 1e6);
        _alive = false;
        return false; // On indique que l'aquisition s'est pas bien passée...
    }

    if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
        std::string error = str(boost::format("Receiver error: %s") % md.strerror());
        _alive = false;
        return false; // On indique que l'aquisition s'est pas bien passée...
    }
#endif
//    for(uint32_t i = 0; i < nSamples; i += 1)
//    {
//        buffer[i] = buffer[i] * 127.0f;
//    }
    //for(int i = 0; i < nSamples; i += 1)
    //{
    //    cout << i << " - " << buffer[i] << std::endl;
    //}
//  std::cout << "[ GOING BACK TO WORK ]" << std::endl;

#if 0

    float*  ptr = (float*)buffer.data();
    for(int i = 0; i< (2 * buffer.size()); i += 1)
    {
        vmin = vmin < ptr[i] ? vmin : ptr[i];
        vmax = vmax > ptr[i] ? vmax : ptr[i];
        vavg += std::abs(ptr[i]);
    }
    vavg /= (2 * buffer.size());
//    std::cout << "vmin = " << vmin << " and vmax = " << vmax << " and vavg = " << vavg << std::endl;
#endif
    return true;
}

void ReceiverUSRP::reset()
{
	usrp.reset();
}

void ReceiverUSRP::start_engine()
{
    std::cout << "[ STARTING USRP ]" << std::endl;
    usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
}

void ReceiverUSRP::stop_engine()
{
    std::cout << "[ STOPPING USRP ]" << std::endl;
    usrp->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
    rx_stream.reset();
}

void ReceiverUSRP::set_rx_gain(const float gain_value)
{
    usrp->set_rx_gain(gain_value);
    std::cout << "[ USRP: Changing reception gain : " << usrp->get_rx_gain() << " dB ]" << std::endl;
}


