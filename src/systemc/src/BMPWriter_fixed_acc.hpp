/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include "systemc.h"
#include "constantes.hpp"
#include "../../Tools/BMP.hpp"
#include <string>
SC_MODULE(BMPWriter_fixed_acc)
{
public:

    sc_fifo_in< sc_uint<32> > addr;
	sc_fifo_in< sc_uint<24> > rgbv;

    SC_CTOR(BMPWriter_fixed_acc)
    {
        SC_THREAD(do_gen);
    }

    ~BMPWriter_fixed_acc( )
    {
        string fn;
        fn = "received_image_fixed_acc";
        string filename = fn + ".bmp";
        if (fopen(filename.c_str(), "rb")== NULL )
          bmp->write( filename.c_str() );
        else
        {
          uint32_t k = 1;
          filename = fn+std::to_string(k)+".bmp";
          while (fopen(filename.c_str(), "rb")!= NULL )
          {
            filename = fn+std::to_string(k)+".bmp";
            k++;
          }
          // const char* filename = ("received_image"+std::to_string(k)+".bmp").c_str();
          bmp->write(filename.c_str());

        }
        delete bmp;
    }

private:
    BMP* bmp;

    void do_gen( )
	{
		bmp = new BMP(640, 480);
		uint8_t* ptr = bmp->data.data();

    //
    // On initilise l'image de sortie dans la couleur rouge pour mieux voir
    // les defauts
    //

    for (uint32_t i = 0; i < (3 * 640 * 480); i += 3)
    {
      ptr[i + 0] = 0xFF;
      ptr[i + 0] = 0x00;
      ptr[i + 0] = 0x00;
    }

		while( true )
        {
            uint32_t adr    = addr.read();
            sc_uint<24> rgb = rgbv.read();

            uint32_t r = rgb.range(23, 16);
            uint32_t g = rgb.range(15,  8);
            uint32_t b = rgb.range( 7,  0);

            if( (3 * adr) >= (3 * 640 *480) )
            {
              std::cout << "(WW) Address value is out of image bounds !" << std::endl;
              continue;
            }

            ptr[3 * adr + 0 ] = r;
            ptr[3 * adr + 1 ] = g;
            ptr[3 * adr + 2 ] = b;
        }
	}

};
