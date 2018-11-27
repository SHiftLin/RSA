#include "stdcpp.h"
#include "Integer.h"
#include "Prime.h"
#include "RSA.h"

ull RandomBigOdd()
{
    ull x = rand() % 8;
    while (x == 0 || x == 7) //000,111
        x = rand() % 8;
    for (int i = 3; i < HLF_RKEY; i++)
        x = (x << 1) | (rand() % 2);
    return (x & 1) ? x : (x | 1);
}

bool MillerRabin(ull a, ull n) //n must be odd
{
    ull x = n - 1, d = x;
    while (!(d & 1))
        d >>= 1;

    ull t = (ull)Int::Pow(a, d, n);

    while (d != x && t != x && t != 1)
        t = (ull)(Int(t) * Int(t) % Int(n)), d <<= 1;
    return (t == x) || (d & 1);
}

bool CheckPrime(ull x)
{
    if (x == 1)
        return false;
    if (x == 2)
        return true;
    for (int i = 0; i < PrimeNum; i++)
        if (x % Prime[i] == 0)
            return false;

    vector<int> a = {2, 3, 7, 61, 24251, 5, 11, 13, 17};
    for (int i = 0; i < a.size(); i++)
        if (a[i] < x && !MillerRabin(a[i], x))
            return false;

    return true;
}

ull RandomPrime()
{
    ull x = RandomBigOdd();
    while (!CheckPrime(x))
        x += 2;
    return x;
}

void ext_gcd(const Int &a, const Int &b, Int &x, Int &y)
{
    if (b == 0)
    {
        x = 1, y = 0;
        return;
    }
    ext_gcd(b, a % b, x, y);
    Int t = x;
    x = y;
    y = t - a / b * y;
}

void RSAKeysGeneration()
{
    int e = 65537;
    ull p, q;
    do
    {
        p = RandomPrime();
        q = RandomPrime();
    } while (p % e == 0 || q % e == 0);

    Int N = Int(p) * Int(q), r = Int(p - 1) * Int(q - 1), d, y;
    ext_gcd(e, r, d, y); //ed-ry=1 <-- ed%r==0
    d = ((d % r) + r) % r;
    RSAKey(N, e).write("./public.key");
    RSAKey(N, d).write("./private.key");

    // cout << "N: " << endl;
    // N.print();
    // cout << endl;

    // cout << "d: " << endl;
    // d.print();
    // cout << endl;

    // cout << "d: " << endl
    //      << e << endl;
}

int main()
{
    srand((unsigned)time(NULL));
    RSAKeysGeneration();
    return 0;
}