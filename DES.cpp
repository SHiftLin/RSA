#include "stdcpp.h"
#include "DES.h"
#include <unistd.h>
#include <fcntl.h>

void DESKeysGeneration(char key[], bool isDecode)
{
    char bits[KEY_SIZE], C[MKEY_SIZE];
    BitsSplit(key, KEY_BYTE, bits);
    Permutate(bits, KEY_SIZE, C, MKEY_SIZE, IPC1);
    for (int i = 0; i < ITER_NUM; i++)
    {
        cshl(C, MKEY_SIZE >> 1, LS[i]);
        cshl(C + (MKEY_SIZE >> 1), MKEY_SIZE >> 1, LS[i]);
        int p = (isDecode) ? ITER_NUM - i - 1 : i;
        Permutate(C, MKEY_SIZE, Keys[p], SKEY_SIZE, IPC2);
    }
}

char *F(char bits[], char Key[])
{
    char B[SKEY_SIZE], C[SKEY_SIZE];
    Permutate(bits, HALF_BLK, C, SKEY_SIZE, PE1);
    XOR(B, C, Key, SKEY_SIZE);
    for (int i = 0, k = 0; i < SKEY_SIZE; i += S_SIZE, k++)
    {
        int r = (B[i] << 1) | B[i + 5], c = 0;
        for (int j = 1; j <= 4; j++)
            c = (c << 1) | B[i + j];
        char temp[1] = {static_cast<char>(S[k][r][c])};
        BitsSplit(temp, 1, C + 4 * k, 4);
    }
    Permutate(C, HALF_BLK, bits, HALF_BLK, PE2);
    return bits;
}

void DES(char bytes[])
{
    char B[BLK_SIZE], C[BLK_SIZE];
    int n = BitsSplit(bytes, BLK_BYTE, B);
    Permutate(B, BLK_SIZE, C, BLK_SIZE, IP1);
    for (int i = 0; i < ITER_NUM; i++)
    {
        char *L = C, *R = C + HALF_BLK;
        Copy(B, R, HALF_BLK);
        XOR(B + HALF_BLK, L, F(R, Keys[i]), HALF_BLK);
        Copy(C, B, BLK_SIZE);
    }
    Switch(C, BLK_SIZE);
    Permutate(C, BLK_SIZE, B, BLK_SIZE, IP2);
    int m = BitsMerge(B, BLK_SIZE, bytes);
}

void RandomDESKey(char key[])
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < KEY_BYTE; i++)
    {
        int x = rand() % 3;
        if (x == 0)
            key[i] = '0' + rand() % 10;
        else if (x == 1)
            key[i] = 'A' + rand() % 26;
        else
            key[i] = 'a' + rand() % 26;
    }
    key[KEY_BYTE] = '\0';
}

FileInfo getInfo(const string &path, bool isDecode, FILE *fin)
{
    char bytes[1024];
    if (fin == NULL)
        WriteError("Input file does not exist!");
    if (isDecode)
    {
        fread(bytes, 1, 10, fin);
        ull size = BytesToInteger(bytes, 8);
        int len = BytesToInteger(bytes + 8, 2);
        fread(bytes, 1, len, fin);
        bytes[len] = '\0';
        string name = bytes;
        return FileInfo(name, size);
    }
    else
    {
        ull size = getFileSize(path.c_str());
        if (size == -1)
            WriteError("Input file error!");
        int p = path.rfind('/');
        string name = (p == -1) ? path : path.substr(p + 1);
        return FileInfo(name, size);
    }
}

void WriteInfo(const FileInfo &info, FILE *fout)
{
    unsigned char bytes[1024];
    IntegerToBytes(info.size, bytes, 8);
    fwrite(bytes, 1, 8, fout);
    IntegerToBytes(info.name.size(), bytes, 2);
    fwrite(bytes, 1, 2, fout);
    fwrite(info.name.c_str(), 1, info.name.size(), fout);
}

int RunDES(string input, string output, char key[], bool randomKey = true, bool isDecode = false)
{
    FileInfo info;
    char block[BLK_BYTE];

    if (input.size() == 0 || (isDecode && randomKey))
        WriteUsage();

    FILE *fin = fopen(input.c_str(), "r");
    info = getInfo(input.c_str(), isDecode, fin);

    if (output.size() == 0)
        output = (isDecode) ? info.name : info.name + ".des";
    else if (output[output.size() - 1] == '/')
        output += (isDecode) ? info.name : info.name + ".des";

    if (access(output.c_str(), F_OK) != -1)
        WriteError(("Output file " + output + " has existed!").c_str());

    FILE *fout = fopen(output.c_str(), "w");
    if (!isDecode)
        WriteInfo(info, fout);

    if (randomKey)
        RandomDESKey(key);
    DESKeysGeneration(key, isDecode);

    for (int cnt = 0, tot = 0, len; (cnt = fread(block, 1, BLK_BYTE, fin)) != 0;)
    {
        tot += cnt;
        for (int i = cnt; i < BLK_BYTE; i++)
            block[i] = 0;
        DES(block);
        if (isDecode && tot >= info.size)
            len = BLK_BYTE - (tot - info.size);
        else
            len = BLK_BYTE;
        fwrite(block, 1, len, fout);
    }

    printf("%s\n", output.c_str());
    printf("Size: %lldB\n", info.size);

    fclose(fin);
    fclose(fout);
    return 0;
}
