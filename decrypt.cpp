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

    RSAKey pub("private.key");

    FILE *fin = fopen("DESKey.rsa", "r");
    unint buf[BUF_LEN];
    fread(buf, 1, BUF_SIZE, fin);
    Int n(buf, BUF_LEN);
    fclose(fin);

    char DESKey[KEY_BYTE + 5];
    Int deKey = Int::Pow(n, pub.x, pub.N);
    deKey.toBytes(DESKey, KEY_BYTE);

    printf("###################\n");
    printf("#  Key: %8s  #\n", DESKey);
    printf("###################\n");

    RunDES(input, output, DESKey, false, true);
    return 0;
}
