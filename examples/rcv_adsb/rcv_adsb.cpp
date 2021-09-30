#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"

#include <iostream>

#include <cmath>
#include <complex>
#include <cstring>
#include <chrono>
#include <bitset>

#include <vector>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <signal.h>

#include "type_aircraft.hpp"

#include "../../src/Tools/Parameters/Parameters.hpp"
#include "../../src/Tools/CTickCounter/CTickCounter.hpp"
#include "../../src/Tools/Conversion/cvt_float_u8.hpp"

#include "../../src/Radio/Receiver/Library/ReceiverLibrary.hpp"

//#define _TIME_PROFILE_

//
//  CplxModule des nombres complexes => module flottant
//

#include "../../src/Processing/CplxModule/Library/CplxModuleLibrary.hpp"
#include "../../src/Processing/Detector/Library/DetectorLibrary.hpp"
//#include "../../src/Chains/ADBS-like/Decoder/Decoder_ADBS_like_chain.hpp"
//#include "../../src/Chains/ADBS-like-fec/Decoder/Decoder_ADBS_FEC_chain.hpp"
#include "../../src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.hpp"
#include "../../src/Processing/DataPacking/BitPacking/BitPacking.hpp"
#include "../../src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.hpp"
#include "../../src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.hpp"


//
//  Correlateur permettant de détecter le prologue des trames ADSB
//

//#include "../../src/Frame/Frame.hpp"
#include "../../src/Processing/Sampling/Down/DownSampling.hpp"
#include "../../src/Processing/PPM/demod/PPM_demod.hpp"

#include "../../src/couleur.h"

long double toRadians(const long double degree) {
    long double one_deg = (M_PI) / 180;
    return (one_deg * degree);
}


double cprMod(double a, double b)
{
    double res = fmod(a,b);
    if (res < 0.0)
        res = res + b;
    return res;
}


double cprNL(const double _lat_)
{
    const double lat = std::abs( _lat_ );

    double n;
    if (lat < 10.47047130)
        n = 59;
    else if (lat < 14.82817437)
        n = 58;
    else if (lat < 18.18626357)
        n = 57;
    else if (lat < 21.02939493)
        n = 56;
    else if (lat < 23.54504487)
    n = 55;
    else if (lat < 25.82924707)
    n = 54;
    else if (lat < 27.93898710)
    n = 53;
    else if (lat < 29.91135686)
    n = 52;
    else if (lat < 31.77209708)
    n = 51;
    else if (lat < 33.53993436)
    n = 50;
    else if (lat < 35.22899598)
    n = 49;
    else if (lat < 36.85025108)
    n = 48;
    else if (lat < 38.41241892)
    n = 47;
    else if (lat < 39.92256684)
    n = 46;
    else if (lat < 41.38651832)
    n = 45;
    else if (lat < 42.80914012)
    n = 44;
    else if (lat < 44.19454951)
    n = 43;
    else if (lat < 45.54626723)
    n = 42;
    else if (lat < 46.86733252)
    n = 41;
    else if (lat < 48.16039128)
    n = 40;
    else if (lat < 49.42776439)
    n = 39;
    else if (lat < 50.67150166)
    n = 38;
    else if (lat < 51.89342469)
    n = 37;
    else if (lat < 53.09516153)
    n = 36;
    else if (lat < 54.27817472)
    n = 35;
    else if (lat < 55.44378444)
    n = 34;
    else if (lat < 56.59318756)
    n = 33;
    else if (lat < 57.72747354)
    n = 32;
    else if (lat < 58.84763776)
    n = 31;
    else if (lat < 59.95459277)
    n = 30;
    else if (lat < 61.04917774)
    n = 29;
    else if (lat < 62.13216659)
    n = 28;
    else if (lat < 63.20427479)
    n = 27;
    else if (lat < 64.26616523)
    n = 26;
    else if (lat < 65.31845310)
    n = 25;
    else if (lat < 66.36171008)
    n = 24;
    else if (lat < 67.39646774)
    n = 23;
    else if (lat < 68.42322022)
    n = 22;
    else if (lat < 69.44242631)
    n = 21;
    else if (lat < 70.45451075)
    n = 20;
    else if (lat < 71.45986473)
    n = 19;
    else if (lat < 72.45884545)
    n = 18;
    else if (lat < 73.45177442)
    n = 17;
    else if (lat < 74.43893416)
    n = 16;
    else if (lat < 75.42056257)
    n = 15;
    else if (lat < 76.39684391)
    n = 14;
    else if (lat < 77.36789461)
    n = 13;
    else if (lat < 78.33374083)
    n = 12;
    else if (lat < 79.29428225)
    n = 11;
    else if (lat < 80.24923213)
    n = 10;
    else if (lat < 81.19801349)
    n = 9;
    else if (lat < 82.13956981)
    n = 8;
    else if (lat < 83.07199445)
    n = 7;
    else if (lat < 83.99173563)
    n = 6;
    else if (lat < 84.89166191)
    n = 5;
    else if (lat < 85.75541621)
    n = 4;
    else if (lat < 86.53536998)
    n = 3;
    else if (lat < 87.00000000)
    n = 2;
    else
    n = 1;
    return n;
}

unsigned char reverse_byte(unsigned char x)
{
    static const unsigned char table[] = {
            0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
            0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
            0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
            0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
            0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
            0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
            0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
            0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
            0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
            0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
            0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
            0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
            0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
            0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
            0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
            0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
            0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
            0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
            0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
            0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
            0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
            0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
            0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
            0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
            0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
            0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
            0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
            0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
            0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
            0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
            0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
            0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };
    return table[x];
}

//static inline void flipbit(uint8_t *buffer, const uint32_t bit_loc ){
//    buffer[bit_loc >> 4] ^= (1 << (bit_loc & 0x7));
//}

long double distance(long double lat1, long double long1,
                     long double lat2, long double long2) {
    lat1  = toRadians(lat1);
    long1 = toRadians(long1);
    lat2  = toRadians(lat2);
    long2 = toRadians(long2);

    // Haversine Formula
    long double dlong = long2 - long1;
    long double dlat = lat2 - lat1;

    long double ans = pow(sin(dlat / 2), 2) +
                      cos(lat1) * cos(lat2) *
                      pow(sin(dlong / 2), 2);

    ans = 2 * asin(sqrt(ans));

    // Radius of Earth in
    // Kilometers, R = 6371
    // Use R = 3956 for miles
    long double R = 6371;

    // Calculate the result
    ans = ans * R;

    return ans;
}



template<int offset, int nbBits>
uint32_t extract_bits(const uint32_t data)
{
    const uint32_t mask  = 0xFFFFFFFF >> (32 - nbBits);
    const uint32_t shift = data >> offset;
    return mask & shift;
}

static unsigned char lookup[16] = {
        0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
        0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

uint8_t reverse(uint8_t n) {
    // Reverse the top and bottom nibble then swap them.
    return (lookup[n&0b1111] << 4) | lookup[n>>4];
}

template<int offset, int nbBits>
uint32_t extract_bits(const uint8_t data)
{
    const uint32_t mask  = 0xFF >> (8 - nbBits);
    const uint32_t shift = reverse(data) >> offset;
    return mask & shift;
}

int32_t pack_bits(const uint8_t *data, const int32_t n) {
    int32_t resu = 0;
    int32_t nm1 = n - 1;
    for (int q = 0; q < n; q++) {
        resu |= (data[q] << (nm1 - q));
    }
    return resu;
}

float pack_bits_float(const uint8_t *data, const int32_t n) {
    int32_t resu = 0;
    int32_t nm1 = n - 1;
    for (int q = 0; q < n; q++) {
        resu |= (data[q] << (nm1 - q));
    }
    return (float)resu / (float)(1 << n);
}

int32_t cprNL(float x) {

    if (x == 0) {
        return 59;
    } else {
        if (x == 87) {
            return 2;
        } else {
            if (x < 0) {
                x = -x;
            }
            if (x > 87) {
                return 1;
            } else {
                const double PIx2 = 2.0 * M_PI;
                const float c = cos(M_PI * x / 180);
                const int i = floor(PIx2 / (acos(1.0 - 0.00547810463 / (c * c))));
                return i;
            }
        }
    }
}

bool CRC(const uint8_t trame[120])
{
    bitset<25> crc       = 0b0000000000000000000000000;
    bitset<25> polynome  = 0b1001000000101111111111111;

    uint32_t zero = 0;

    for (int q = 8; q < 33; q++) {
        crc[q - 8] = trame[q];
        zero      += trame[q];
    }

    for (int q = 33; q < 120; q++) {
        if (crc[0] == 1) {
            crc = crc xor polynome;
        }
        crc = crc >> 1;
        crc[24] = trame[q];
        zero      += trame[q];
    }

    if (crc[0] == 1) {
        crc = crc xor polynome;
    }
    crc &= 0x00FFFFFF;

    return (crc == 0x00000000) && (zero != 0);
}

uint32_t CRC_LUT[256];
const uint32_t CRC_POLY = 0x00FFF409;

void init_crc_lut()
{
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i << 16;
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 0x800000) {
                crc = ((crc << 1) ^ CRC_POLY) & 0xffffff;
            } else {
                crc = (crc << 1) & 0xffffff;
            }
        }
        CRC_LUT[i] = crc & 0xffffff;
    }
}

template<int length>
uint32_t check_crc(const uint8_t *msg/*, const uint32_t length*/) {
    uint32_t i;
    uint32_t calc_crc = 0;
    for (i = 0; i < length; i++) {
        calc_crc = CRC_LUT[((calc_crc >> 16) ^ ((uint32_t) msg[i])) & 0xff] ^ (calc_crc << 8);
    }
//    uint32_t rx_crc = (((uint32_t)msg[length - 3]) << 16) | (((uint32_t)msg[length -2]) << 8) | ((uint32_t)msg[length -1]);
//    if( !(rx_crc ^ calc_crc) && (rx_crc != 0)){
//        return 1;
//    }
    return (calc_crc & 0xffffff) == 0;
}

static inline void flipbit(uint8_t *buffer, uint32_t bit_loc ){
//    buffer[bit_loc >> 3] ^= (1 << (bit_loc & 0x7));
    buffer[bit_loc / 8] ^= (1 << (7 - bit_loc%8));
}

template<int length>
uint32_t check_crc_2(const uint8_t *msg/*, const uint32_t length*/) {
    uint32_t calc_crc = 0;
    for (uint32_t i = 0; i < length - 3; i++) {
        calc_crc = CRC_LUT[((calc_crc >> 16) ^ ((uint32_t) msg[i])) & 0xff] ^ (calc_crc << 8);
    }
    calc_crc = (calc_crc & 0xffffff); // ARG !!!
    const uint32_t rx_crc = (((uint32_t)msg[length - 3]) << 16) | (((uint32_t)msg[length -2]) << 8) | ((uint32_t)msg[length -1]);
    if( !(rx_crc ^ calc_crc) && (rx_crc != 0)){
        return 1;
    }
    return 0;
}


float ComputeLatitude(const float f_latitude, const float ref_latitude, const int32_t CPR_format)
{
    const float d_latitude = 360.0 / (4.0 * 15.0 - (float)CPR_format);
    const float j = std::floor(ref_latitude/d_latitude) + std::floor(0.5 + cprMod(ref_latitude,d_latitude)/d_latitude - f_latitude);
    const float final_lat = d_latitude * (j + f_latitude);
    return final_lat;
}

float ComputeLongitude(const float f_longitude, const float final_lat, const float ref_longitude, const int32_t CPR_format)
{
    const float NL = cprNL(final_lat);
    const float cdiff = NL - ((double)CPR_format);
    float Dlon;
    if ( cdiff > 0.0 )
    {
        Dlon = 360.0 / (NL - (float)CPR_format);
    }
    else
    {
        Dlon = 0;
    }
    const float m   = floor(ref_longitude/Dlon) + floor(0.5 + cprMod(ref_longitude, Dlon)/Dlon - f_longitude);
    const float final_lon = Dlon*(m + f_longitude);
    return final_lon;
}


float Latitude(const uint8_t trame[120])
{
    float latref = 44.806884;    //latitude de l'ENSEIRB
    float Dlat = 360.0 / (4.0 * 15.0 - trame[39 + 22]);
    float lat = 0;
    int jlat = 0;
    int ref_Dlat = floor(latref / Dlat);
    for (int q = 0; q < 17; q++) {
        lat += trame[39 + 23 + q] * pow(2, 16 - q);
    }
    lat = lat / 131072.0f; // divise par 2^17
    jlat = ref_Dlat + floor(0.5 + latref / Dlat - ref_Dlat - lat);
    float latitude = Dlat * (jlat + lat);
    return latitude;
}


/*   ============================== LONGITUDE =========================== */
float Longitude(const uint8_t trame[120], float latitude)
{
    float lonref = -0.606629;    //longitude de l'ENSEIRB
    //cout << "cprNL :" << cprNL(latitude) << endl;
    float Dlon;
    int tempNL = cprNL(latitude) - trame[39 + 22];
    if (tempNL != 0) Dlon = 360 / (float)tempNL;
    else Dlon = 360;
    float lon = 0;
    int jlon = 0;
    int ref_Dlon = floor(lonref / Dlon);
    for (int q = 0; q < 17; q++) {
        lon += trame[39 + 40 + q] * pow(2, 16 - q);
    }
    lon = lon / 131072;
    jlon = ref_Dlon + floor(0.5 + lonref / Dlon - ref_Dlon - lon);
    float longitude = Dlon * (jlon + lon);

    return longitude;
}

using namespace std;

bool isFinished = false;

void my_ctrl_c_handler(int s) {
    if (isFinished == true) {
        exit(EXIT_FAILURE);
    }
    isFinished = true;
}

void show(std::vector<float> &v) {
    const uint32_t ll = v.size();
    for (uint32_t i = 0; i < ll; i += 1) {
        if (i % 16 == 0)
            printf("\n");
        printf("%1.3f ", v[i]);
    }
    printf("\n");
}

#include "Avion.hpp"

/*   ============================== MAIN =========================== */
/*
	4 ech = 1 symb
	1 trame = 120 symb = 480 ech
*/
int main(int argc, char *argv[]) {
    init_crc_lut();

#if 0
    const float ref_latitude  = 44.806884; // latitude  de l'ENSEIRB
    const float ref_longitude = -0.606629; // longitude de l'ENSEIRB
#else
    const float ref_latitude  = 44.820919; // latitude  de la maison
    const float ref_longitude = -0.502448; // longitude de la maison
#endif

    map<int32_t, Avion*> liste_m;
    vector<Avion *> liste_v;

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    uint32_t nbTramesDetectees = 0; // nbre trame ...
    uint32_t nbBonsCRCs      = 0;
    uint32_t nbBonsCRC_init  = 0;
    uint32_t nbBonsCRCs_1x   = 0;
    uint32_t nbBonsCRCs_2x   = 0;
    uint32_t nbBonsCRCs_llr  = 0;
    uint32_t nbStrangeFrames = 0;

    int c; //getopt

    Parameters param;

    param.set("mode_radio", "radio");
    param.set("filename", "usrp");

    param.set("fc", 1090000000.0);
    param.set("fe", 4000000.0);

    param.set("hackrf_amplifier", -1);

    param.set("receiver_gain", -1);

#if defined(__ARM_NEON)
    param.set("mode_conv", "NEON"); // scalar
    param.set("mode_corr", "NEON"); // scalar
#elif defined(__AVX2__)
    param.set("mode_conv", "AVX2"); // scalar
    param.set("mode_corr", "AVX2"); // scalar
#else
    param.set("mode_conv", "scalar"); // scalar
    param.set("mode_corr", "scalar"); // scalar
#endif
    
    param.set("payload", 60);

    param.set("verbose", 0);

    param.set("ps_min", 0.75f);

    param.set("mode_inter", true);
    param.set("dump_frame", false);

    param.set("crystal_correct", 0);

    param.set("list_mode", false);

    param.set("brute_force_1x", false);
    param.set("brute_force_2x", false);
    param.set("brute_force_llr",false);


    static struct option long_options[] =
            {
                    {"verbose",     no_argument,       NULL, 'v'},  // affiche temps sur chaque boucle Np + cplx => abs
                    {"seuil",       required_argument, NULL, 's'},  // pour changer la valeur min de la correlation (synchro)
                    {"np",          required_argument, NULL, 'n'},  // pour changer le nombre de boucle Np (ie nbre echantillon*200000) // Np = 10 => 0.5 s

                    {"conv",        required_argument, NULL, 'c'}, // a partir d'un fichier
                    {"corr",        required_argument, NULL, 'd'}, // a partir d'un fichier

                    {"radio",       required_argument, NULL, 'r'}, // a partir d'un fichier
                    {"file",        required_argument, NULL, 'F'}, // a partir d'un fichier
                    {"file-stream", required_argument, NULL, 'Q'}, // a partir d'un fichier

                    {"fc",          required_argument, NULL, 'f'}, // changer la frequence de la porteuse
                    {"fe",          required_argument, NULL, 'e'}, // changer la frequence echantillonnage

                    {"payload",     required_argument, NULL, 'p'}, // changer la frequence echantillonnage

                    {"amplifier",   required_argument, NULL, 'A'}, // changer la frequence echantillonnage
                    {"rcv_gain",    required_argument, NULL, 'L'}, // changer la frequence echantillonnage
                    {"rcv-gain",    required_argument, NULL, 'L'}, // changer la frequence echantillonnage
                    {"brute-force-1x",  no_argument,       NULL, '1'}, // changer la frequence echantillonnage
                    {"brute-force-2x",  no_argument,       NULL, '2'}, // changer la frequence echantillonnage
                    {"brute-force-llr", no_argument,       NULL, 'X'}, // changer la frequence echantillonnage

                    {"inter",       no_argument,       NULL, 'I'}, // changer la frequence echantillonnage
                    {"intra",       no_argument,       NULL, 'i'}, // changer la frequence echantillonnage
                    {"dump-frames", no_argument,       NULL, 'D'}, // changer la frequence echantillonnage
                    {"ppm",         required_argument, NULL, 'P'}, // changer la frequence echantillonnage
                    {"liste",       no_argument,       NULL, 'l'}, // changer la frequence echantillonnage

                    {NULL, 0,                          NULL, 0}
            };

    int option_index = 0;

    cout << "par Florian LOUPIAS - Février 2020" << endl;
    cout << "par Bertrand LE GAL - Octobre 2020" << endl;
    cout << "==================================== ADSB ====================================" << endl;
    // ============== GETOPT ================
    printf("%s", KRED);

    while ((c = getopt_long(argc, argv, "be:p:f:n:s:vt8", long_options, &option_index)) != -1) {
        switch (c) {
            case 0:
                printf("%soption %s%s", long_options[option_index].name, KNRM, KRED);
                if (optarg)
                    printf("%s with arg %s%s", optarg, KNRM, KRED);
                printf("\n");
                break;

            case 'f':
                param.set("fc", std::stod(optarg));
                break;

            case 'e' :
                param.set("fe", std::stoi(optarg));
                break;

            case 'A' :
                param.set("hackrf_amplifier", std::stoi(optarg));
                break;

            case 'L' :
                param.set("receiver_gain", std::stoi(optarg));
                break;

            case 'p':
                param.set("payload", std::atoi(optarg));
                break;

            case 's':
                param.set("ps_min", std::stof(optarg));
                if ((param.toFloat("ps_min") > 1.0f) || (param.toFloat("ps_min") < 0.0f)) {
                    printf("erreur : --produit_scalaire (ps_min) doit etre compris entre 0 et 1\n");
                    exit(EXIT_FAILURE);
                }
                break;

            case 'v':
                param.set("verbose", param.toInt("verbose") + 1);
                break;

            case '?':
                break;

            case 'c':
                param.set("mode_conv", optarg);
                break;

            case 'd':
                param.set("mode_corr", optarg);
                break;

            case 'r':
                param.set("mode_radio", "radio");
                param.set("filename", optarg);
                break;

            case 'F':
                param.set("mode_radio", "file");
                param.set("filename", optarg);
                break;

            case 'Q':
                param.set("mode_radio", "file-stream");
                param.set("filename", optarg);
                break;

            case '1':
                param.set("brute_force_1x", true);
                break;

            case '2':
                param.set("brute_force_2x", true);
                break;

            case 'X':
                param.set("brute_force_llr", true);
                break;

            case 'I':
                param.set("mode_inter", true);
                break;

            case 'i':
                param.set("mode_inter", false);
                break;

            case 'D':
                param.set("dump_frame", true);
                break;

            case 'l':
                param.set("list_mode", true);
                break;

            case 'P' :
                param.set("crystal_correct", std::atoi(optarg));
                printf("%soption crystal_correct = %d dB%s\n", KNRM, param.toInt("crystal_correct"), KRED);
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
                exit(EXIT_FAILURE);
                break;
        }
    }
    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
        exit(EXIT_FAILURE);
    }
    printf("%s", KNRM);
    cout << endl;

    // param.toDouble("fe")/8
    vector<complex<float> > buffer(65536); // Notre buffer à nous dans le programme

    //
    // Selection du module SDR employé dans le programme
    //
    Receiver *radio = ReceiverLibrary::allocate(param);

    //
    // Selection du module de correlation employé dans le programme
    //
    Detector *detect = DetectorLibrary::allocate(param);


    //
    // Selection du module de conversion employé dans le programme
    //
    CplxModule *conv = CplxModuleLibrary::allocate(param);


//    Frame f( param.toInt("payload") );

    printf("(II) Launching the emitter application :\n");
    printf("(II) -> Modulation frequency    : %4d MHz\n", (uint32_t) (param.toDouble("fc") / 1000000.0));
    printf("(II) -> Symbol sampling freq.   : %4d MHz\n", (uint32_t) (param.toDouble("fe") / 1000000.0));
    printf("(II)\n");

    const int32_t crystal_correct_ppm = param.toInt("crystal_correct");
    if (crystal_correct_ppm != 0) {
        double freq_hz = param.toDouble("fc");
        double sample_rate_hz = param.toDouble("fe");

        sample_rate_hz = (uint32_t) ((double) sample_rate_hz * (1000000.0 - crystal_correct_ppm) / 1000000.0 + 0.5);
        freq_hz = freq_hz * (1000000.0 - crystal_correct_ppm) / 1000000.0;

        param.set("fc", freq_hz);
        param.set("fe", sample_rate_hz);

        printf("(II) -> Corrected modulation frequency    : %4d MHz\n", (uint32_t) (param.toDouble("fc") / 1000000.0));
        printf("(II) -> Corrected symbol sampling freq.   : %4d MHz\n", (uint32_t) (param.toDouble("fe") / 1000000.0));
        printf("(II)\n");
    }

    printf("(II) HackRF module configuration :\n");
    printf("(II) -> HackRF antenna parameter   : enable\n");
    printf("(II) -> HackRF amplifier parameter : disable\n");


    //
    // Selection du module de conversion employé dans le programme
    //


    //
    // Selection du module de conversion employé dans le programme
    //

    radio->initialize();
    radio->start_engine();

    auto start = std::chrono::system_clock::now();


    //
    // Selection du module de conversion employé dans le programme
    //

    CTickCounter timer;

    const float ps_min     = param.toFloat("ps_min");

    const bool brute_force_1x  = param.toBool("brute_force_1x");
    const bool brute_force_2x  = param.toBool("brute_force_2x");
    const bool brute_force_llr = param.toBool("brute_force_llr");

    uint32_t loop_counter = 0;


    std::vector<float> buffer_abs;
    std::vector<float> buffer_detect;

    DownSampling down(2);
    PPM_demod ppd;

    std::vector<uint8_t> buff_6;
    std::vector<uint8_t> buff_7;

#if 0
    BMP* bmp = nullptr;
    int32_t i_width  = -1;
    int32_t i_height = -1;
    int32_t curr_x   =  0;
    int32_t curr_y   =  0;

    uint32_t nBytesPerPixel = 0;
    uint32_t nBytesPerLine = 0;
#endif

    vector<float>   vec_data (480);
    vector<float>   vec_down (240);
    vector<uint8_t> vec_demod(120);
    vector<uint8_t> vec_sync (112);
    vector<uint8_t> vec_pack(14);

    DownSampling      o_down(2);
    PPM_demod         o_ppm;
    RemoveADSBSynchro o_sync;
    BitPacking        o_pack;
    CheckCRC32b       o_ccrc;
    RemoveCRC32b      c_rcrc;


//    Decoder_chain* dec_chain;
//    dec_chain = new Decoder_ADBS_like_chain( F.size_frame() );

    std::vector<uint8_t> frame_buf(112);


#ifdef _TRACE_MODE_
    std::ofstream of( "dec_frames.txt" );
#endif

//#define _DUMP_ALL_SIGNALS_


#ifdef _DUMP_ALL_SIGNALS_
    std::vector<uint8_t> frame_status( buffer_detect.size() );
#endif

    const bool StoreDataSet = param.toBool("dump_frame");

    FILE* file_iq_raw     = nullptr; // Le fichier contenant les données I/Q
    FILE* file_frames_bin = nullptr; // Le fichier contenant les 112 bits correspondant aux frames
    FILE* file_frames_dec = nullptr; // Le fichier contenant les trames décodées
    FILE* file_planes     = nullptr; // Le fichier contenant les informations concernant les avions
    FILE* file_coords     = nullptr; // Le fichier contenant les informations concernant les avions

    if( StoreDataSet == true )
    {
        file_iq_raw     = fopen( "file_iq_raw.cs8", "w" );
        file_frames_bin = fopen( "file_frames_bin.txt", "w" );
        file_frames_dec = fopen( "file_frames_dec.txt", "w" );
        file_planes     = fopen( "file_planes.txt", "w" );
        file_coords     = fopen( "file_coords.txt", "w" );
    }

    bool firstAcq = true;
//    const bool mode_inter = param.toBool("mode_inter");

    uint32_t stream_ptr = 0;

    const uint32_t verbose = param.toInt("verbose");

#if 0
    vector<int32_t> histo(65536);
    for (int i = 0; i < histo.size(); i++) histo[i] = 0;
#endif

    printf("+---------------+--------+--------+-----+--------+-----+----------+-------------+------+--------+-----+-----+-----+---------+---------+-------+-----+\n");
    printf("| Frame | Acqui | Echan. |  Corr. |  DF |   AA   | FTC |    CS    | ALT (ft) | CPRF | LON (deg) | LAT (deg) | Dis | Speed.H | Speed.V | Angle | CRC |\n");
    printf("+---------------+--------+--------+-----+--------+-----+----------+-------------+------+--------+-----------+-----+---------+---------+-------+-----+\n");

    if( file_frames_dec != nullptr )
    {
        fprintf(file_frames_dec, "+---------------+--------+--------+-----+--------+-----+----------+-------------+------+--------+-----+-----+-----+---------+---------+-------+-----+\n");
        fprintf(file_frames_dec, "| Frame | Acqui | Echan. |  Corr. |  DF |   AA   | FTC |    CS    | ALT (ft) | CPRF | LON (deg) | LAT (deg) | Dis | Speed.H | Speed.V | Angle | CRC |\n");
        fprintf(file_frames_dec, "+---------------+--------+--------+-----+--------+-----+----------+-------------+------+--------+-----------+-----+---------+---------+-------+-----+\n");
    }

    while (radio->alive() && (isFinished == false))
    {
//        auto startIter = std::chrono::system_clock::now();

        uint32_t coverage = vec_data.size();
        coverage = (firstAcq == true) ? 0 : coverage;
        firstAcq = false;

#if 0
        printf("(II) RADIO RECEPTION (stream = %8u | nSample = %8lu)\n", stream_ptr, buffer.size() - coverage);
#endif

        stream_ptr += buffer.size() - coverage;
#if 0
        int caff = cnt%64;
        if( caff ==   0 ) printf("Reception .   \r");
        if( caff ==  16 ) printf("Reception ..  \r");
        if( caff ==  32 ) printf("Reception ... \r");
        if( caff ==  48 ) printf("Reception ....\r");
        fflush(stdout);
        cnt += 1;
#endif
        timer.start_loading();
        bool error_n = radio->reception(buffer, coverage);
        timer.stop_loading();

        if (error_n == false)    // Cela signifie que l'on a rencontré une erreur lors de la
            continue;                   // reception des echantillons

        //
        // CALCUL DU MODULE DES VOIES (I,Q)
        //
        timer.start_conversion();
        conv->execute(&buffer, &buffer_abs);
        timer.stop_conversion();

        //
        // ON LANCE LA FONCTION DE DETECTION SUR L'ENSEMBLE DU BUFFER
        //
        timer.start_detection();
        detect->execute(&buffer_abs, &buffer_detect);
        timer.stop_detection();

        const int dump_decoded_frame = true;
        const int dump_resume = param.toBool("list_mode");

        //
        // ON VA MAINTENANT PARCOURIR LE TABLEAU DE SCORE POUR DETECTER LES TRAMES
        //
        const uint32_t length = (buffer_abs.size() - coverage);
        for (uint32_t k = 0; k < length; k += 1) {

            float score = buffer_detect[k];
            if (score > ps_min)
            {

                //
                // Le score de la position k depasse le score minimum fixé par l'utilisateur
                //

                timer.start_decoding();
                nbTramesDetectees += 1;    // On vient de detecter qqchose

                //
                // On stocke les données pour une utilisation ultérieure
                //
                if( file_iq_raw != nullptr )
                {
                    int8_t dSet[960];
                    float* iSet = (float*)(buffer.data() + k);
                    float maxv = 0;
                    for (int x = 0; x < 960; x += 1)
                        maxv = std::max(maxv, iSet[x]);  // On suppose que les données que l'on récupere sont  -127 <=> +127
                    for (int x = 0; x < 960; x += 1)
                        dSet[x] = (maxv <= 1.0f) ? 127.0f * iSet[x] : iSet[x];  // On suppose que les données que l'on récupere sont  -127 <=> +127
                    fwrite(dSet, sizeof(int8_t), 960, file_iq_raw);
                    for (int x = 0; x < 60; x += 1) // On rajoute un peu de data entre 2 trames
                        dSet[x] = (rand()%6) - 3;
                    fwrite(dSet, sizeof(int8_t), 60, file_iq_raw);
                }

                for (int x = 0; x < vec_data.size(); x += 1)    // On extrait les 120 bits (x2) du vecteur
                    vec_data[x] = buffer_abs[x + k];            // d'echantillons (modules complexes de IQ)

                o_down.execute(vec_data, vec_down);         // 480 values => 240 values
                o_ppm.execute (vec_down, vec_demod);     // 240 values => 120 bits
                o_sync.execute(vec_demod, vec_sync);     // 112 bits
                o_pack.execute(vec_sync, vec_pack);         // 15 octets

                const bool crc_initial = check_crc  <112 / 8>( vec_pack.data() );
#if _LOUPIAS_CRC_TEST_
                bool ok_slow  = CRC(vec_demod.data());
                if ( ok_slow != ok_fast )
                {
                    red();
                    printf(" [ THERE IS SOMETHING STRANGE ])\n");
                    black();
                    printf("  "); for(int32_t i = 0; i < vec_demod.size(); i += 1) printf("%d", vec_demod[i]); printf("\n");
                    printf("  "); for(int32_t i = 0; i < vec_sync.size(); i += 1) printf("%d ", vec_sync[i] ); printf("\n");
                    printf("  "); for(int32_t i = 0; i < vec_pack.size(); i += 1) printf("%2.2X", vec_pack[i]); printf("\n");
                    printf("  brute_crc_flo = %d\n", ok_slow  );
                    printf("  check_crc     = %d\n", ok_fast  );
                }
#endif
                //
                // On stocke les données pour une utilisation ultérieure
                //
                if( file_frames_bin != nullptr ) {
                         if(crc_initial ) fprintf(file_frames_bin, "%5d | OK | [", nbTramesDetectees);
                    else            fprintf(file_frames_bin, "%5d | KO | [", nbTramesDetectees);
                    for (int x = 0; x < vec_sync.size() - 1; x += 1)
                        fprintf(file_frames_bin, "%d,", vec_sync[x]);
                    fprintf(file_frames_bin, "%d]\n", vec_sync[vec_sync.size() - 1]);
                }

                bool crc_brute_1x = false;
                if ( (crc_initial == false) && (brute_force_1x == true) )
                {
                    const int32_t stop_1_bit = 112;
                    for (int j = 0; j < stop_1_bit; j += 1)
                    {
                        flipbit( vec_pack.data(), j );

                        crc_brute_1x = check_crc<112 / 8>( vec_pack.data() );
                        if (crc_brute_1x == true)
                        {
                            vec_demod[j + 8] = !vec_demod[j + 8];       // On inverse le bit
                            vec_sync [j    ] = !vec_sync [j    ];       // On inverse le bit

#if 0
                            yellow();
                            printf("%6d : %6d | [ !!! BIT FLIP TECHNIQUE SAVED A FRAME !!! ] \n", loop_counter, k);
                            black();
#endif
                            break;
                        }

                        flipbit( vec_pack.data(), j );
                    }
                }

#if 0
                if ( (ok_fast == false) && (brute_force == true) )
                {
                    const int32_t stop_1_bit = 112; // Cela implique que l'on corrige un bit dans le CRC si nécessaire
                    for (int j = 0; j < stop_1_bit; j += 1)
                    {
                        vec_demod[j] = !vec_demod[j]; // On inverse le bit

                        const bool brute_crc_flo = CRC(vec_demod.data());

                        if (brute_crc_flo == true)
                        {
                            o_sync.execute(vec_demod, vec_sync); // 112 bits
                            o_pack.execute(vec_sync, vec_pack);    //  15 octets
                            const bool brute_crc = check_crc<112 / 8>(vec_pack.data());

                            if (brute_crc == false) {
                                red();
                                printf(" [THERE IS SOMETHING WRONG] (brute_crc_flo = %d and brute_crc = %d and bit-flip = %d)\n", brute_crc_flo, brute_crc, j);
                                black();
                            }else{
                                yellow();
                                printf("%6d : %6d | [ !!! BIT FLIP TECHNIQUE SAVED A FRAME !!! ] \n", loop_counter, k);
                                black();
                                nbSavedCRCs += 1;
                                ok_fast      = true;
                                o_pack.execute(vec_sync, vec_pack);
                                break;
                            }
                        }
                        vec_demod[j] = !vec_demod[j]; // On re-inverse le bit
                    }
                }
#endif

                
#define _BRUTE_FORCE_2x_
                bool crc_brute_2x = false;
#ifdef _BRUTE_FORCE_2x_
                if ( (crc_initial == false) && (crc_brute_1x == false) && (brute_force_2x == true) )
                {
                    const int32_t stop_1_bit = 112;      //
                    const int32_t stop_2_bit = 112 - 24;

                    for (int ii = 0; ii < stop_1_bit; ii += 1)
                    {
                        flipbit( vec_pack.data(), ii );

                        for (int jj = ii + 1; jj < stop_2_bit; jj += 1)
                        {
                            flipbit( vec_pack.data(), jj );

                            crc_brute_2x  = check_crc<112 / 8>( vec_pack.data() );
                            if (crc_brute_2x == true)
                            {
                                vec_demod[ii + 8] = !vec_demod[ii + 8]; // On inverse le bit
                                vec_sync [ii    ] = !vec_sync [ii    ]; // On inverse le bit
                                vec_demod[jj + 8] = !vec_demod[jj + 8]; // On inverse le bit
                                vec_sync [jj    ] = !vec_sync [jj    ]; // On inverse le bit
#if 0
                                magenta();
                                printf("%6d : %6d | [ !!! 2x BIT FLIP TECHNIQUE SAVED A FRAME !!! ] \n", loop_counter, k);
                                black();
#endif
//                                o_pack.execute(vec_sync, vec_pack);
                                break;
                            }

                            flipbit( vec_pack.data(), jj );
                        }

                        if( crc_brute_2x == true ) break;

                        flipbit( vec_pack.data(), ii );
                    }
                }
#endif

                
#define _BRUTE_FORCE_LLR_
#ifdef  _BRUTE_FORCE_LLR_
                bool crc_brute_llr = false;
                if ( (crc_initial == false) && (crc_brute_1x == false) && (crc_brute_2x == false) && (brute_force_llr == true)  )
                {
                    std::vector<float> vec_score (112);
                    for(int32_t ii = 16; ii < 240; ii += 2)
                    {
                        const float diff = vec_down[ii] - vec_down[ii + 1];
                        const float abso = std::abs(diff);
                        vec_score[(ii / 2) - 8] = abso;
                    }

                    sort(vec_score.begin(), vec_score.end());

                    for(int32_t ii = 0; ii < vec_score.size(); ii += 1)
                        printf("%3d : (%17.13f)\n", ii, vec_score[ii]);
                    
                    crc_brute_llr = true;

                    exit( 0 );
                }
#endif

                const bool crc_is_ok  = ((crc_initial == true)  || (crc_brute_1x == true) || (crc_brute_2x == true));
                nbBonsCRCs     += crc_is_ok;
                nbBonsCRC_init += (crc_initial   == true);
                nbBonsCRCs_1x  += (crc_brute_1x  == true);
                nbBonsCRCs_2x  += (crc_brute_2x  == true);
                nbBonsCRCs_llr += (crc_brute_llr == true);

                if ((crc_is_ok == true) || (crc_is_ok == false && verbose >= 2))
                {
                    char* crc_show;
                         if( crc_initial   == true ) crc_show = "\x1B[32mOK\x1B[0m";
                    else if( crc_brute_1x  == true ) crc_show = "\x1B[33mOK\x1B[0m";
                    else if( crc_brute_2x  == true ) crc_show = "\x1B[35mOK\x1B[0m";
                    else if( crc_brute_llr == true ) crc_show = "\x1B[36mOK\x1B[0m";
//                    printf("%d %d %d %d => %s\n", crc_initial, nbBonsCRCs_1x, nbBonsCRCs_2x, nbBonsCRCs_llr, crc_show);

                    if( verbose >= 1 )
                    {
                        printf("  [");
                        for(int32_t i = 0; i < vec_sync.size(); i += 1)
                            printf("%d ", vec_sync[i] );
                        printf("];\n");
                    }

                    if (crc_is_ok)
                    {

                        const int32_t df_value   = pack_bits(vec_sync.data(), 5);
                        const int32_t type_frame = pack_bits(vec_sync.data() + 32, 5);
                        const int32_t oaci_value = pack_bits(vec_sync.data() + 8, 24);

                        if( df_value != 17 )
                        {
                            nbStrangeFrames += 1;
                            continue;
                        }

                        Avion *ptr_avion = liste_m[oaci_value];
                        if (ptr_avion == nullptr) {
                            ptr_avion = new Avion(oaci_value);
                            liste_m[oaci_value] = ptr_avion;
                            liste_v.push_back(ptr_avion);
                        }
                        ptr_avion->update();
                        ptr_avion->set_score( score );

                        if ((type_frame >= 1) && (type_frame <= 4))
                        {
                            const int32_t type_airc = pack_bits(vec_sync.data() + 37, 3);
                            const int32_t typeAricraft = toCode( type_frame, type_airc );
                            if( typeAricraft == -1 )
                            {
                                nbStrangeFrames += 1;
                                continue;
                            }

                            const char lut[] = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######";
                            char caractere[9];
                            caractere[0] = lut[ pack_bits(vec_sync.data() + 40, 6) ];
                            caractere[1] = lut[ pack_bits(vec_sync.data() + 46, 6) ];
                            caractere[2] = lut[ pack_bits(vec_sync.data() + 52, 6) ];
                            caractere[3] = lut[ pack_bits(vec_sync.data() + 58, 6) ];
                            caractere[4] = lut[ pack_bits(vec_sync.data() + 64, 6) ];
                            caractere[5] = lut[ pack_bits(vec_sync.data() + 70, 6) ];
                            caractere[6] = lut[ pack_bits(vec_sync.data() + 76, 6) ];
                            caractere[7] = lut[ pack_bits(vec_sync.data() + 82, 6) ];
                            caractere[8] = 0;

                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d | %s |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, caractere, crc_show);
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d | %s |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, caractere);

                            ptr_avion->set_type(typeAricraft);
                            ptr_avion->set_name(caractere);
                        }

                        if ((type_frame >= 5) && (type_frame <= 8) )
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show);
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if ((type_frame >= 9) && (type_frame <= 18) )
                        {
                            const int32_t upper = pack_bits(vec_sync.data() + 40, 7);
                            const int32_t incr  = pack_bits(vec_sync.data() + 47, 1);
                            const int32_t lower = pack_bits(vec_sync.data() + 48, 4);
                            int32_t altitude = (upper << 4) | lower;
                            altitude = ( incr == 1) ? (altitude * 25 - 1000) : -1;

                            //
                            // On extrait les informations de la trame
                            //
                            const int32_t CPR_format    = pack_bits(vec_sync.data() + 53, 1);
                            const float f_latitude      = pack_bits_float(vec_sync.data() + 54, 17);//(float)enc_latitude  / 131072.0; // divise par 2^17
                            const float f_longitude     = pack_bits_float(vec_sync.data() + 71, 17);//(float)enc_longitude / 131072.0; // divise par 2^17

                            //
                            // On calcule les parametres reels
                            //

                            float final_lat    = ComputeLatitude (f_latitude,  ref_latitude,                CPR_format);
                            float final_lon    = ComputeLongitude(f_longitude, final_lat,    ref_longitude, CPR_format);
                            const int32_t dist = distance(final_lat, final_lon, ref_latitude, ref_longitude);


                            if (dump_decoded_frame && (dump_resume == false)) {
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |   %6d |    %d |  %8.5f |  %8.5f | %3d |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t)altitude, CPR_format, final_lon, final_lat, dist, crc_show);
                            }
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |   %6d |    %d |  %8.5f |  %8.5f | %3d |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t)altitude, CPR_format, final_lon, final_lat, dist);

                            ptr_avion->set_altitude (altitude);
                            ptr_avion->set_GNSS_mode(false);
                            ptr_avion->set_latitude (final_lat);
                            ptr_avion->set_longitude(final_lon);
                            ptr_avion->update_distance();
                        }
#if 1
                        if (type_frame == 19)
                        {
                            if ((vec_demod[39 + 6] == 0) & (vec_demod[39 + 7] == 0) & (vec_demod[39 + 8] == 1)) {
                                // Type 1
                                // vitesse horizontale
                                int vNS = 0;
                                for (int q = 0; q < 10; q++) vNS += vec_demod[39 + 26 + q] * pow(2, 10 - q);
                                if (vec_demod[39 + 25] == 1) vNS = 1 - vNS;
                                else vNS = vNS - 1;
                                int vEW = 0;
                                for (int q = 0; q < 10; q++) vEW += vec_demod[39 + 15 + q] * pow(2, 10 - q);
                                if (vec_demod[39 + 25] == 1) vEW = 1 - vEW;
                                else vEW = vEW - 1;
                                float speed = sqrt(vEW * vEW + vNS * vNS);  // en kt ?? plutot en km/h
                                //speed = speed * 1.852;
                                // angle
                                float angle = atan2(vEW, vNS) * 360 / (2 * M_PI);
                                // vitesse verticale
                                int Vr = 0;
                                for (int q = 0; q < 8; q++) Vr += vec_demod[39 + 38 + q] * pow(2, 8 - q);
                                Vr = (Vr - 1) * 64;
                                if (vec_demod[39 + 37] == 1) Vr = -Vr; // en feet/min
                                Vr = Vr * 0.3048;
                                if (dump_decoded_frame && (dump_resume == false))
                                    printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |    %4d |    %4d |  %4d |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t) speed, (int32_t) Vr, (int32_t) angle, crc_show);
                                if( file_frames_dec != nullptr )
                                    fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |    %4d |    %4d |  %4d |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t) speed, (int32_t) Vr, (int32_t) angle);
                                ptr_avion->set_speed_horizontal(speed);
                                ptr_avion->set_speed_vertical(Vr);
                                ptr_avion->set_angle(angle);
                            }
                        }
#endif
#if 1
                        if ((type_frame >= 20) && (type_frame <= 22) )
                        {
                            int32_t altitude = 0;
                            for (int q = 0; q < 12; q++) {
                                if (q > 8)
                                    altitude += vec_demod[39 + 9 + q] * pow(2, 10 - q - 1);
                                else {
                                    if (q < 8)
                                        altitude += vec_demod[39 + 9 + q] * pow(2, 10 - q); }
                            }
                            altitude = altitude * 25 - 1000;

                            const int32_t CPR_format  = pack_bits(vec_sync.data() + 53, 1);
                            const float f_latitude    = pack_bits_float(vec_sync.data() + 54, 17);//(float)enc_latitude  / 131072.0; // divise par 2^17
                            const float f_longitude   = pack_bits_float(vec_sync.data() + 71, 17);//(float)enc_longitude / 131072.0; // divise par 2^17
                            const float final_lat     = ComputeLatitude (f_latitude,  ref_latitude,                CPR_format);
                            const float final_lon     = ComputeLongitude(f_longitude, final_lat,    ref_longitude, CPR_format);
                            const int32_t dist        = distance(final_lat, final_lon, ref_latitude, ref_longitude);

                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |   %6d |    %d |  %8.5f |  %8.5f | %3d |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t)altitude, CPR_format, final_lon, final_lat, dist, crc_show);
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |   %6d |    %d |  %8.5f |  %8.5f | %3d |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t)altitude, CPR_format, final_lon, final_lat, dist);
                            ptr_avion->set_altitude (altitude);
                            ptr_avion->set_GNSS_mode(true);
                            ptr_avion->set_latitude (final_lat);
                            ptr_avion->set_longitude(final_lon);
                            ptr_avion->update_distance();
                        }
#endif
                        if ((type_frame >= 23) && (type_frame <= 27))
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show);
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if (type_frame == 28)
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show);
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if (type_frame == 29)
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show);
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if (type_frame == 31)
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show);
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if( (dump_decoded_frame == false) && (dump_resume == false) )
                        {
                            ptr_avion->print();
                        }
#if 0
                        for(int i = 0; i < vec_demod.size(); i += 1)
                        {
                            if(i ==  8) printf(" ");
                            else if(i == 13) printf(" ");
                            else if(i == 16) printf(" ");
                            else if(i == 40) printf(" ");
                            else if(i == 96) printf(" ");
                            printf("%d", vec_demod[i]);
                        }
                        printf("\n");
#endif
                    } else {
                        for (int i = 0; i < vec_demod.size(); i += 1) {
                            if (i == 8) printf(" ");
                            else if (i == 13) printf(" ");
                            else if (i == 16) printf(" ");
                            else if (i == 40) printf(" ");
                            else if (i == 96) printf(" ");
                            printf("%d", vec_demod[i]);
                        }
                        printf(" ");

                        int oaci = 0;
                        for (int q = 0; q < 24; q++) {
                            oaci += vec_demod[16 + q] * pow(2, 23 - q);
                        }
                        printf(" [OACI : %06X] ", oaci);

                        red();
                        printf(" [CRC KO]\n");
                        black();
                    }

                }
                timer.stop_decoding();
                k += 1; // On pourrait augementer de plus car il y a peu de chance qu'une autre
                        // trame soit présente d'ici 480 ech.
            }
        }
        loop_counter += 1;


        if ( (dump_resume == true) && ( (loop_counter % 32 == 0) || (!radio->alive()) || (isFinished == true)) )
        {
            printf("\e[1;1H\e[2J");
            const auto stop = std::chrono::system_clock::now();
            const int32_t eTime = chrono::duration_cast<chrono::seconds>(stop - start).count();
            std::cout << "Application runtime : " << eTime << " seconds"
                      << std::endl;
            std::cout << "Number of frames over the detection value      : " << nbTramesDetectees << std::endl;
            std::cout << "Number of frames with validated CRC value      : " << nbBonsCRCs        << std::endl;
            std::cout << " - Number of initially correct  CRC values     : " << nbBonsCRC_init    << std::endl;
            std::cout << " - Number of saved frames with bit-flip (x1)   : " << nbBonsCRCs_1x     << std::endl;
            std::cout << " - Number of saved frames with bit-flip (x2)   : " << nbBonsCRCs_2x     << std::endl;
            std::cout << "Number of discarded frames with strange values : " << nbStrangeFrames   << " (type != 17)" <<std::endl;
            nbStrangeFrames += 1;

            std::cout << std::endl;
            printf("OACI   | TYPE   | NAME     | CORR. SCORE       |  LATITUDE | LONGITUDE | H.SPEED   | V.SPEED   | ANGLE | TYPE | ALTITUDE  | DISTANCE [min,max] | FRAMES | LAST     |\n");
            printf("-------+--------+----------+-------------------+-----------+-----------+-----------+-----------+-------+------+-----------+--------------------+--------+----------|\n");
            for (uint32_t i = 0; i < liste_v.size(); i += 1)
            {
                if( liste_v.at(i)->get_messages() >= 1 )    // Pour filter les bétises...
                    liste_v.at(i)->print();
            }
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << "MESSAGES:" << std::endl;
            printf("|      n       |  t (in s)  |  Corr. |  DF |   AA   | FTC |    CS    | ALT (ft) | CPRF | LON (deg) | LAT (deg) | Speed.H | Speed.V | Angle | CRC |\n");
        }
        //
        // ON GARDE UNE TRACE DU TEMPS D'EXECUTION DE L'ITERATION POUR L'HISTOGRAMME DU REPORT
        //
    }

    //
    // https://mobisoftinfotech.com/tools/plot-multiple-points-on-map/
    //
    if( file_coords != nullptr )
    {
        for (uint32_t ii = 0; ii < liste_v.size(); ii += 1)    // pour tous les avions
        {
            for (uint32_t jj = 0; jj < liste_v.at(ii)->list_long.size(); jj += 1)    // pour tous les positions
            {
                fprintf(file_coords, "%24.22f,%24.22f,#00FF00\n", liste_v.at(ii)->list_lat[jj], liste_v.at(ii)->list_long[jj]);
            }
        }
    }

    printf("\n================================================================\n");
    std::cout << "loading    : " << timer.loading()    << std::endl;
    std::cout << "conversion : " << timer.conversion() << std::endl;
    std::cout << "detection  : " << timer.detection()  << std::endl;
    std::cout << "decoding   : " << timer.decoding()   << std::endl;
    printf("================================================================\n");

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double RTConstr = 1000.0f / (param.toDouble("fe") / buffer.size());
    double avgTime = chrono::duration_cast<chrono::milliseconds>(elapsed).count() / (float) loop_counter;

    printf("\n");
    std::cout << "Nombre d'aquisition réalisées  : " << loop_counter << std::endl;
    std::cout << "Temps moyen par acquisition    : " << avgTime << " ms" << std::endl;
    std::cout << "Constrainte tps réel / iter    : " << RTConstr << " ms" << std::endl;

    std::cout << "Number of frames over the detection value      : " << nbTramesDetectees << std::endl;
    std::cout << "Number of frames with validated CRC value      : " << nbBonsCRCs        << std::endl;
    std::cout << " - Number of initially correct  CRC values     : " << nbBonsCRC_init    << std::endl;
    std::cout << " - Number of saved frames with bit-flip (x1)   : " << nbBonsCRCs_1x     << std::endl;
    std::cout << " - Number of saved frames with bit-flip (x2)   : " << nbBonsCRCs_2x     << std::endl;
    std::cout << "Number of discarded frames with strange values : " << nbStrangeFrames   << " (type != 17)" <<std::endl;

    printf("\n");
    std::cout << "Nombre de trames emises  (frames)  = " << nbBonsCRCs << std::endl;

    printf("\n");
    std::cout << "Temps total : " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << std::endl;
#ifdef _TIME_PROFILE_
    std::cout << " -  timer_reception     = " << (timer_reception/1000/1000)     << " ms" << std::endl;
    std::cout << " -  timer_conv_cplx     = " << (timer_conv_cplx/1000/1000)     << " ms" << std::endl;
    std::cout << " -  timer_detecteur     = " << (timer_detecteur/1000/1000)     << " ms" << std::endl;
    std::cout << " -  timer_extraction    = " << (timer_extraction/1000/1000)    << " ms" << std::endl;
    std::cout << " -  timer_down_sampling = " << (timer_down_sampling/1000/1000) << " ms" << std::endl;
    std::cout << " -  timer_demodulator   = " << (timer_demodulator/1000/1000)   << " ms" << std::endl;
    std::cout << " -  timer_parsing       = " << (timer_parsing/1000/1000)       << " ms" << std::endl;
#endif
    printf("\n");

#if 0
    int sumv = 0;
    for (int i = 0; i < histo.size(); i++) {
        sumv += histo[i];
    }
    sumv = (sumv == 0) ? 1 : sumv;
    int first = 0;
    for (int i = 0; i < histo.size(); i++) {
        if (histo[i] != 0) break;
        first = i;
    }
    int last = 0;
    for (int i = 0; i < histo.size(); i++) {
        if (histo[histo.size() - 1 - i] != 0) break;
        last = histo.size() - 1 - i;
    }
    for (int i = 0; i < histo.size(); i++) {
        histo[i] = (10000 * histo[i]) / sumv;
    }
    for (int i = first; i <= last; i++) {
        printf("%4d - %1.3f\n", i, ((float) histo[i]) / 100.0f);
    }
#endif

    if( StoreDataSet == true )
    {
        fclose(file_iq_raw    );
        fclose(file_frames_bin);
        fclose(file_frames_dec);
        fclose(file_planes    );
        fclose(file_coords    );
    }

    delete radio;
    delete detect;
    delete conv;

    return 0;
}

#pragma clang diagnostic pop
