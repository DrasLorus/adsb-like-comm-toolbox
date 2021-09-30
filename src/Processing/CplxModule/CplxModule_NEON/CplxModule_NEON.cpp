#include "CplxModule_NEON.hpp"

#if defined(__ARM_NEON)
    #include <arm_neon.h>
#endif


CplxModule_NEON::CplxModule_NEON()
{

}

CplxModule_NEON::~CplxModule_NEON()
{

}


void CplxModule_NEON::execute(std::vector< std::complex<float> >* buffer_in, std::vector<float>* buffer_out)
{
#if defined(__ARM_NEON)
    if( buffer_in->size() != buffer_out->size() )
    {
      buffer_out->resize( buffer_in->size() );
    }

    const uint32_t length = buffer_in->size();

    float* ptr_i = (float*)buffer_in->data();
    float* ptr_o = (float*)buffer_out->data();

    //
    // Boucle automatiquement vectorisée avec clang / macos apple silicon
    //
    for(uint32_t kk = 0; kk < length; kk += 1)
    {
        const float breal = ptr_i[2 * kk + 0];
        const float bimag = ptr_i[2 * kk + 1];
        const float resul = sqrt( breal * breal + bimag * bimag);
        ptr_o[kk] = resul;
    }
#else
    exit( EXIT_FAILURE );
#endif
}
