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
