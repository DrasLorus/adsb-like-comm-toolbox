#ifndef _my_module1_
#define _my_module1_

#include "systemc.h"
#include "ModuleCompute_old.hpp"
#include "Detecteur1.hpp"
#include "DownSampling.hpp"
#include "BitDecider.hpp"
#include "BitsToBytes.hpp"
#include "CRCCheck.hpp"
#include "FrameProcessing.hpp"


SC_MODULE(my_module1)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_int <8>  > e;
    sc_fifo_out< sc_uint<32> > addr;
    sc_fifo_out< sc_uint<24> > rgbv;
    // sc_fifo_out< bool > detect1;



	SC_CTOR(my_module1) :
	    mod("mod"),
//      dbl("dbl"),
	    det("det"),
        dow("dow"),
        bit("bit"),
        byt("byt"),
        crc("crc"),
        fra("fra"),
        mod2dbl("mod2dbl", 1024),
//        dbl2det1("dbl2det1", 32),
//        dbl2det2("dbl2det2", 32),
        det2dow("det2dow", 32),
        dow2bit("dow2bit", 32),
        bit2byt("bit2byt", 32),
        byt2crc("byt2crc", 32),
        crc2fra("crc2fra", 32)

	{
       // cout<<"Modcompute starting "<<endl;
	      mod.clock( clock );
        mod.reset( reset );
        mod.e( e );
        mod.s(mod2dbl );
        // mod.s2(mod2det2 );
       // cout<<"Modcompute ending "<<endl;
       // cout<<"Modcompute starting "<<endl;
//       dbl.clock( clock );
//       dbl.reset( reset );
//       dbl.e(mod2dbl);
//       dbl.s1(dbl2det1);
//       dbl.s2(dbl2det2);
       // cout<<"Modcompute ending "<<endl;
        det.clock( clock );
        det.reset( reset );
        det.e(mod2dbl);
//        det.e2(dbl2det2);
        det.s(det2dow);
        // det.detect1(detect1);



        dow.clock( clock );
        dow.reset( reset );
        dow.e(det2dow);
        dow.s(dow2bit);



        bit.clock( clock );
        bit.reset( reset );
        bit.e(dow2bit);
        bit.s(bit2byt);



        byt.clock( clock );
        byt.reset( reset );
        byt.e(bit2byt);
        byt.s(byt2crc);



        crc.clock( clock );
        crc.reset( reset );
        crc.e(byt2crc);
        crc.s(crc2fra);



        fra.clock( clock );
        fra.reset( reset );
        fra.e(crc2fra);
        fra.addr(addr);
        fra.rgbv(rgbv);


    }

private:
    ModuleCompute   mod;
//    DOUBLEUR_U        dbl;
    Detecteur1       det;
    DownSampling    dow;
    BitDecider      bit;
    BitsToBytes     byt;
    CRCCheck        crc;
    FrameProcessing fra;

    sc_fifo< sc_uint<8> > mod2dbl;
    sc_fifo< sc_uint<8> > dbl2det1;
    sc_fifo< sc_uint<8> > dbl2det2;
    sc_fifo< sc_uint<8> > det2dow;
    sc_fifo< sc_uint<8> > dow2bit;
    sc_fifo< sc_uint<1> > bit2byt;
    sc_fifo< sc_uint<8> > byt2crc;
    sc_fifo< sc_uint<8> > crc2fra;


};

#endif
