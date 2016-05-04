#include "mabimt.h"

MabiMT::MabiMT()
{
}

void MabiMT::Init(unsigned int s) {
    _mt[0] = s & 0xffffffff;
    for (_mti = 1; _mti < N; _mti++) {
        _mt[_mti] = (unsigned int)(1812433253 * (_mt[_mti - 1] ^ (_mt[_mti - 1] >> 30)) + _mti);
        _mt[_mti] &= 0xffffffff;
    }
}

unsigned int MabiMT::GenRand() {
    unsigned int y;
    if (_mti >= N) {
        int kk;
        if (_mti == N + 1) Init(5489);
        for (kk = 0; kk < N - M; kk++) {
            y = (_mt[kk] & UpperMask) | (_mt[kk + 1] & LowerMask);
            _mt[kk] = _mt[kk + M] ^ (y >> 1) ^ Mag01[y & (unsigned int)0x1];
        }

        for (; kk < N - 1; kk++) {
            y = (_mt[kk] & UpperMask) | (_mt[kk + 1] & LowerMask);
            _mt[kk] = _mt[kk + (M - N)] ^ (y >> 1) ^ Mag01[y & (unsigned int)0x1];
        }

        y = (_mt[N - 1] & UpperMask) | (_mt[0] & LowerMask);
        _mt[N - 1] = _mt[M - 1] ^ (y >> 1) ^ Mag01[y & (unsigned int)0x1];

        _mti = 0;
    }
    y = _mt[_mti++];

    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680;
    y ^= (y << 15) & 0xefc60000;
    y ^= (y >> 18);

    return y;
}
