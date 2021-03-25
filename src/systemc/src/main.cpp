#include "RAWReader.hpp"
#include "my_module.hpp"
#include "my_module1.hpp"
#include "my_module2.hpp"
#include "Tripleur.hpp"
#include "BMPWriter.hpp"
#include "BMPWriter_fixed.hpp"
#include "BMPWriter_fixed_acc.hpp"

// #include "UART_COM.h"
// #include "ETHERNET_COM.h"



#include <iostream>
#include <math.h>

using namespace std;

int main (int argc, char * argv []){
	cout << "Initialisation des composants..." << endl;

    RAWReader raw ("RAWReader");
		Tripleur trpl ("Tripleur");
		// my_module_fixed_acc mod2 ("my_module_fixed_acc");
		my_module2 mod2 ("my_module2");
		my_module1 mod1 ("my_module1");
		my_module mod ("my_module");


    BMPWriter bmp1 ("BMPWriter");
		BMPWriter_fixed bmp2 ("BMPWriter_fixed");
		BMPWriter_fixed_acc bmp3 ("BMPWriter_fixed_acc");


		// BMPWriter_optim bmp_optim ("BMPWriter_optim");
		// UART_COM uart_com ("UART_com");
		// ETHERNET_COM ethernet_com ("ETHERNET_com");

    sc_clock clk("clk",10,SC_NS);
    sc_signal<bool> reset;



		reset = false;

    sc_fifo< sc_int <8>  > raw2trpl  (16);
		sc_fifo< sc_int <8>  > trpl2mod1  (16);
		sc_fifo< sc_int <8>  > trpl2mod2  (16);
		sc_fifo< sc_int <8>  > trpl2mod3  (16);

		sc_fifo< sc_uint<32> > mod2bmp_1facc (16);
		sc_fifo< sc_uint<24> > mod2bmp_2facc (16);
    sc_fifo< sc_uint<32> > mod2bmp_1f (16);
    sc_fifo< sc_uint<24> > mod2bmp_2f (16);
		sc_fifo< sc_uint<32> > mod2bmp_1 (16);
		sc_fifo< sc_uint<24> > mod2bmp_2 (16);
		// sc_fifo< bool > detect1 (4096);


    raw.s(raw2trpl);

		// uart_com.e(raw2mod);
		// ethernet_com.e(raw2mod);
		trpl.clock(clk);
		trpl.reset(reset);
		trpl.e(raw2trpl);
		trpl.s1(trpl2mod1);
		trpl.s2(trpl2mod2);
		trpl.s3(trpl2mod3);

		mod2.clock  ( clk );
	 	mod2.reset(reset);
	 	mod2.e(trpl2mod1);
	 	mod2.addr(mod2bmp_1facc);
	 	mod2.rgbv(mod2bmp_2facc);

 		mod1.clock  ( clk );
		mod1.reset(reset);
    mod1.e(trpl2mod2);
    mod1.addr(mod2bmp_1f);
    mod1.rgbv(mod2bmp_2f);
		// mod1.detect1(detect1);
		//
		mod.clock  ( clk );
		mod.reset(reset);
		mod.e(trpl2mod3);
		mod.addr(mod2bmp_1);
		mod.rgbv(mod2bmp_2);

    bmp1.addr(mod2bmp_1);
    bmp1.rgbv(mod2bmp_2);

		bmp2.addr(mod2bmp_1f);
    bmp2.rgbv(mod2bmp_2f);

		bmp3.addr(mod2bmp_1facc);
		bmp3.rgbv(mod2bmp_2facc);


//	ldpc.clk  ( clk );
//	ldpc.reset(reset);

    sc_start();

	return 0;
}
