/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _Detecteur1_
#define _Detecteur1_

#include "systemc.h"
#include <cstdint>
#include "Doubleur_uint.hpp"
#include "trames_separ1.hpp"
#include "Seuil_calc1.hpp"

// #define _DEBUG_SYNCHRO_

SC_MODULE(Detecteur1)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_uint<8> > e;
    // sc_fifo_in < sc_uint<8> > e2;
    sc_fifo_out< sc_uint<8> > s;
    // sc_fifo_out< bool > detect1;


	SC_CTOR(Detecteur1):
    s_calc("s_calc"),
    t_sep("t_sep"),
    dbl("dbl"),
    dbl2scalc("dbl2scalc",1024),
    dbl2tsep("dbl2tsep",1024),
    detect("detect", 1024)
//    detect1("detect1", 4096)

	{
    dbl.clock(clock);
    dbl.reset(reset);
    dbl.e(e);
    dbl.s1(dbl2scalc);
    dbl.s2(dbl2tsep);

    s_calc.clock(clock);
    s_calc.reset(reset);
    s_calc.e(dbl2scalc);
    s_calc.detect(detect);
    // s_calc.detect1(detect1);

    t_sep.clock(clock);
    t_sep.reset(reset);
    t_sep.e(dbl2tsep);
    t_sep.detect(detect);
    t_sep.s(s);

	}

private:
    Seuil_calc1 s_calc;
    trames_separ1 t_sep;
    DOUBLEUR_U dbl;


    sc_fifo< sc_uint<8> > dbl2scalc;
    sc_fifo< sc_uint<8> > dbl2tsep;
    sc_fifo <bool> detect;




};

#endif
