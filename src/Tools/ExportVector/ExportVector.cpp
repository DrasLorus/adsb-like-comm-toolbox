/**
  Copyright (c) 2012-2017 "Bordeaux INP, Bertrand LE GAL"
  [http://legal.vvv.enseirb-matmeca.fr]

  This file is part of LDPC_C_Simulator.

  LDPC_C_Simulator is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ExportVector.hpp"

#include "../Conversion/cvt_float_i8.hpp"
#include "../Conversion/cvt_float_u8.hpp"

void ExportVector::SaveToCS8(std::vector<complex<float>>& dSet, std::string filename)
{
    const float* ptr_d = (const float*)dSet.data();
    FILE* f = fopen(filename.c_str(), "wb");
    std::vector<int8_t> i8_dSet( 2 * dSet.size() );
    for(uint32_t i = 0; i < i8_dSet.size(); i += 1 )
        i8_dSet[i] = (*ptr_d++);
    fwrite( i8_dSet.data(), sizeof(int8_t), i8_dSet.size(), f );
    fclose( f );
}

void ExportVector::SaveToS8(std::vector<float>& dSet, std::string filename)
{
    std::vector<uint8_t> u8_dSet;
    cvt_float_u8::execute( &dSet, &u8_dSet );

    FILE* f = fopen(filename.c_str(), "wb");
    fwrite( u8_dSet.data(), sizeof(uint8_t), u8_dSet.size(), f );
    fclose( f );
}


void ExportVector::SaveToU8(std::vector<float>& dSet, std::string filename)
{
    std::vector<int8_t> i8_dSet;
    cvt_float_i8::execute( &dSet, &i8_dSet );

    FILE* f = fopen(filename.c_str(), "wb");
    fwrite( i8_dSet.data(), sizeof(int8_t), i8_dSet.size(), f );
    fclose( f );
}


void ExportVector::SaveToS8(std::vector< int8_t>& dSet, std::string filename)
{
    FILE* f = fopen(filename.c_str(), "wb");
    fwrite( dSet.data(), sizeof(int8_t), dSet.size(), f );
    fclose( f );
}


void ExportVector::SaveToU8(std::vector<uint8_t>& dSet, std::string filename)
{
    FILE* f = fopen(filename.c_str(), "wb");
    fwrite( dSet.data(), sizeof(uint8_t), dSet.size(), f );
    fclose( f );
}


void ExportVector::SaveVector(std::vector<float>& abs, std::vector<uint8_t>& detec, std::string filename)
{
    FILE* f = fopen(filename.c_str(), "wb");
    std::vector<uint8_t> u8_abs( abs.size() );
    for(uint32_t q = 0; q < abs.size(); q += 1)
        u8_abs[q] = abs[q];
    for(uint32_t q = 0; q < abs.size(); q += 1)
    {
        fwrite( &u8_abs[q], 1, sizeof(uint8_t), f );
        fwrite( &detec [q], 1, sizeof(uint8_t), f );
    }
    fclose( f );
}


#if !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
void ExportVector::SaveVector(std::vector<uint8_t>& abs, std::vector<uint8_t>& detec, std::string filename)
{

}

#if !defined(__clang__)
#pragma GCC diagnostic pop
#else
#pragma clang diagnostic pop
#endif