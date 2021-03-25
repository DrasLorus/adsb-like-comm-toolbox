/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
 #ifndef _FrameProcessing_
 #define _FrameProcessing_

#include "systemc.h"
#include <cstdint>
#include "constantes.hpp"

SC_MODULE(FrameProcessing)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_uint< 8> > e;
    sc_fifo_out< sc_uint<32> > addr;
    sc_fifo_out< sc_uint<24> > rgbv;

	SC_CTOR(FrameProcessing)
	{
		SC_CTHREAD(do_gen, clock.pos());
        reset_signal_is(reset,true);
	}

private:

	void do_gen( )
	{
		cout << "(II) RAWReader :: START" << endl;

        uint32_t curr_adr;
//        uint16_t curr_x;
        // uint16_t curr_y;

        while(true)
        {
//            uint8_t  type    = e.read();
//            uint16_t special = e.read();

            uint16_t type = e.read(); type |= ((uint16_t)e.read()) << 8;
            uint16_t mot1 = e.read(); mot1 |= ((uint16_t)e.read()) << 8;
            uint16_t mot2 = e.read(); mot2 |= ((uint16_t)e.read()) << 8;
            uint16_t mot3 = e.read(); mot3 |= ((uint16_t)e.read()) << 8;

            if( type == FRAME_NEW_IMAGE )
            {
                cout << "(II) FrameProcessing :: FRAME_NEW_IMAGE" << endl;
                curr_adr = 0;
                // curr_y   = 0;

                for(uint16_t i = 0; i < _BYTE_PAYLOAD_; i += 1)
                    e.read();
            }
            else if( type == FRAME_END_IMAGE )
            {
                cout << "(II) FrameProcessing :: FRAME_END_IMAGE" << endl;
#ifdef _SIMULATION_
                wait( 10, SC_US );
                sc_stop();
#endif
                curr_adr = 0;
                // curr_y   = 0;

                for(uint16_t i = 0; i < _BYTE_PAYLOAD_; i += 1)
                    e.read();
            }
            // else if( type == FRAME_NEW_LINE )
            // {
            //     sc_uint< 8> r0 = e.read();
            //     sc_uint< 8> r1 = e.read();
            //     sc_uint<16> r2 = (r1, r0);
            //     curr_y   = r2;
            //     curr_adr = _IMAGE_WIDTH_ * curr_y;
            //
            //     for(uint16_t i = 2; i < _BYTE_PAYLOAD_; i += 1)
            //         e.read();
            //     cout << "(II) FrameProcessing :: FRAME_NEW_LINE (" << curr_y << ")" << endl;
            // }
            // else if( type == FRAME_END_LINE )
            // {
            //     sc_uint< 8> r0 = e.read();
            //     sc_uint< 8> r1 = e.read();
            //     sc_uint<16> r2 = (r1, r0);
            //     curr_y   = r2;
            //     curr_adr = _IMAGE_WIDTH_ * curr_y;
            //
            //     for(uint16_t i = 2; i < _BYTE_PAYLOAD_; i += 1)
            //         e.read();
            // }
            else if( type == FRAME_INFOS )
            {
                //
                // On utilise les données provenant de la trame pour calculer
                // la position du bloc de pixel dans l'image
                //
                // cout << mot1 << mot2 << endl;
                uint32_t curr_off = mot1 + _IMAGE_WIDTH_ * mot2;

//                cout << "(II) FrameProcessing :: FRAME_INFOS :: curr_y = " <<  curr_y << " :: special = " << special << " :: curr_off = " << curr_off << endl;

                for(uint16_t i = 0; i < _BYTE_PAYLOAD_; i += 3)
                {
                    sc_uint< 8> R = e.read();
                    sc_uint< 8> G = e.read();
                    sc_uint< 8> B = e.read();
                    sc_uint<24> RGB = (R, G, B);
                    rgbv.write( RGB );
                    addr.write( curr_off );
                    curr_off += 1;
                }
            }
            else
            {
                //
                // Le type de trame n'est pas reconnu, on met directement a la bin les données
                //
                cout << "(EE) Error detected on frame type :: flusing data to the bin..." << endl;
                for(uint16_t i = 0; i < _BYTE_PAYLOAD_ + _BYTE_CRC_; i += 1)
                    e.read();
            }
        }
	}
};
#endif
