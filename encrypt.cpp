#include "stdcpp.h"
#include "Integer.h"
#include "DES.cpp"
#include "RSA.h"

int main(int argc, char **argv)
{
    string input, output;
    for (char ch; (ch = getopt(argc, argv, "i:o:")) != -1;)
        switch (ch)
        {
        case 'i':
            input = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        }

    RSAKey pub("public.key");

    char DESKey[KEY_BYTE + 5];
    RunDES(input, output, DESKey, true, false);
    Int n(BytesToInteger(DESKey, KEY_BYTE));
    Int enKey = Int::Pow(n, pub.x, pub.N);

    FILE *fout = fopen("DESKey.rsa", "w");
    unint buf[BUF_LEN];
    fwrite(enKey.getv(buf, BUF_LEN), 1, BUF_SIZE, fout);
    fclose(fout);

    return 0;
}
