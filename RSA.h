#include "stdcpp.h"
#include "Integer.h"

const int RSAKEY_SIZE = 128;
const int HLF_RKEY = RSAKEY_SIZE >> 1;
const int BUF_LEN = RSAKEY_SIZE / 32;
const int BUF_SIZE = BUF_LEN * sizeof(unint);

struct RSAKey
{
    Int N, x;
    unint buf[BUF_LEN];

    RSAKey(const Int &_N, const Int &_x)
    {
        N = _N;
        x = _x;
    }

    RSAKey(const char *name)
    {
        FILE *fin = fopen(name, "r");
        fread(buf, 1, BUF_SIZE, fin);
        N = Int(buf, BUF_LEN);
        fread(buf, 1, BUF_SIZE, fin);
        x = Int(buf, BUF_LEN);
        fclose(fin);
    }

    void write(const char *name)
    {
        FILE *fout = fopen(name, "w");
        fwrite(N.getv(buf, BUF_LEN), 1, BUF_SIZE, fout);
        fwrite(x.getv(buf, BUF_LEN), 1, BUF_SIZE, fout);
        fclose(fout);
    }
};