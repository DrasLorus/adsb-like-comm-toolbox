#include "cvt_float_u8.hpp"


cvt_float_u8::cvt_float_u8()
{

}


cvt_float_u8::~cvt_float_u8()
{

}


void cvt_float_u8::execute(std::vector<float>* buffer_in, std::vector<uint8_t>* buffer_out)
{
    if( buffer_in->size() != buffer_out->size() )
        buffer_out->resize( buffer_in->size() );

    execute(buffer_in->data(), buffer_out->data(), buffer_out->size());
}


void cvt_float_u8::execute(const float* buffer_in, std::vector<uint8_t>* buffer_out)
{
    execute(buffer_in, buffer_out->data(), buffer_out->size());
}


void cvt_float_u8::execute(const float* buffer_in, uint8_t* buffer_out, const uint32_t n)
{
    for (uint32_t i = 0; i < n; i += 1)
    {
        float value   = 255.0f * buffer_in[i];
        float vmax    = (value >= 255.0f ) ? 255.0f : value;
        float vmin    = (vmax  <=   0.0f ) ?   0.0f :  vmax;
        buffer_out[i] = vmin;
    }
}
