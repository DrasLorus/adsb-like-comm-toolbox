#ifndef _tools_
#define _tools_

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

void show(std::vector<float> &v) {
    const uint32_t ll = v.size();
    for (uint32_t i = 0; i < ll; i += 1) {
        if (i % 16 == 0)
            printf("\n");
        printf("%1.3f ", v[i]);
    }
    printf("\n");
}

#endif