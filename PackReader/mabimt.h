/*
 MabiMe Character Simulator - by Yai (Sophie N)
 Email: sinoc300@gmail.com
 Copyright (C) 2016

 This program is free software: you can redistribute it and/or modify
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

#ifndef MABIMT_H
#define MABIMT_H


class MabiMT
{
public:
    MabiMT();
    void Init(unsigned int s);
    unsigned int GenRand();
private:
    static const int N = 624, M = 397;
    static const unsigned int MatrixA = 0x9908b0df, UpperMask = 0x80000000, LowerMask = 0x7fffffff;
    unsigned int Mag01[2] = {0, MatrixA};
    unsigned int _mt[N];
    int _mti = N + 1;
};

#endif // MABIMT_H
