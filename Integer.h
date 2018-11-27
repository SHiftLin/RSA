#ifndef INTEGER_H
#define INTEGER_H

#include "stdcpp.h"
#include "Util.h"

class Int
{
    ull v[12]; //not unint
    int len;
    bool flag;
    static const ull base = ull(0xffffffff) + 1;

    Int copy(int n) const
    {
        Int res;
        res.len = min(len, n);
        for (int i = 0; i < res.len; i++)
            res.v[i] = v[len - res.len + i];
        return res;
    }

    void shl()
    {
        len++;
        for (int i = len - 1; i >= 1; i--)
            v[i] = v[i - 1];
        v[0] = 0;
    }

    void remove()
    {
        while (len > 1 && v[len - 1] == 0)
            len--;
    }

  public:
    Int()
    {
        memset(v, 0, sizeof(v));
        len = flag = 0;
    }

    Int(ull x)
    {
        flag = false;
        memset(v, 0, sizeof(v));
        for (len = 0; x > 0; x /= base)
            v[len++] = x % base;
        if (len == 0)
            len++;
    }

    Int(char *buf, int n) {}

    Int(unint *buf, int n)
    {
        len = n;
        flag = false;
        for (int i = 0; i < n; i++)
            v[i] = buf[i];
        remove();
    }

    char *toBytes(char *buf, int n)
    {
        n /= sizeof(unint);
        for (int i = 0; i < len; i++)
            IntegerToBytes(v[len - i - 1], (unsigned char *)(buf + (i * sizeof(unint))), sizeof(unint));
        return buf;
    }

    unint *getv(unint *buf, int n)
    {
        for (int i = 0; i < n; i++)
            buf[i] = v[i];
        return buf;
    }

    static Int rev(Int x)
    {
        x.flag = !x.flag;
        return x;
    }

    static Int abs(Int x)
    {
        if (x.flag)
            x.flag = !x.flag;
        return x;
    }

    bool operator==(const Int &rhs) const
    {
        if (flag != rhs.flag)
            return false;
        if (len != rhs.len)
            return false;
        for (int i = len; i >= 0; i--)
            if (v[i] != rhs.v[i])
                return false;
        return true;
    }

    bool operator<(const Int &rhs) const
    {
        if (flag && !rhs.flag)
            return true;
        if (!flag && rhs.flag)
            return false;
        if (len < rhs.len)
            return true;
        if (len > rhs.len)
            return false;
        for (int i = len; i >= 0; i--)
        {
            if (v[i] < rhs.v[i])
                return true;
            if (v[i] > rhs.v[i])
                return false;
        }
        return false;
    }

    bool operator<=(const Int &rhs) const
    {
        return (*this) < rhs || (*this) == rhs;
    }

    Int operator+(const Int &rhs) const
    {
        if (flag != rhs.flag)
        {
            if (flag && !rhs.flag)
                return rhs - rev(*this);
            else
                return (*this) - rev(rhs);
        }
        Int res;
        res.flag = flag;
        res.len = max(len, rhs.len) + 1;
        for (int i = 0, c = 0; i < res.len; i++)
        {
            ull x = (i >= len) ? 0 : v[i], y = (i >= rhs.len) ? 0 : rhs.v[i];
            ull z = x + y + c;
            res.v[i] = z % base;
            c = z / base;
        }
        res.remove();
        return res;
    }

    Int operator-(const Int &rhs) const //rhs<this
    {
        if (flag != rhs.flag)
            return (*this) + rev(rhs);
        if (flag && rhs.flag)
            return rev(rhs) - rev(*this);

        Int res;
        const Int *a = this, *b = &rhs; //a-b
        if ((*this) < rhs)
        {
            res.flag = true;
            swap(a, b);
        }
        for (int i = 0, c = 0; i < a->len; i++)
        {
            ll y = (i >= b->len) ? 0 : b->v[i];
            ll x = (ll)a->v[i] - y - c;
            if (x < 0)
                x += base, c = 1;
            else
                c = 0;
            res.v[i] = x;
        }
        res.len = a->len;
        res.remove();
        return res;
    }

    ull Find(const Int &rhs, const Int &rod) const
    {
        if (rod < rhs)
            return 0;
        ull low = 0, high = base - 1;
        while (low < high)
        {
            ull mid = ((low + high + 1) >> 1);
            if ((rhs * mid) <= rod)
                low = mid;
            else
                high = mid - 1;
        }
        return low;
    }

    Int operator*(const Int &rhs) const
    {
        Int res;
        res.flag = flag ^ rhs.flag;
        for (int i = 0; i < len; i++)
            for (int j = 0; j < rhs.len; j++)
            {
                res.v[i + j] += v[i] * rhs.v[j];
                res.v[i + j + 1] += res.v[i + j] / base;
                res.v[i + j] %= base;
            }
        res.len = len + rhs.len;
        res.remove();
        return res;
    }

    Int &operator*=(const Int &rhs)
    {
        *this = (*this) * rhs;
        return *this;
    }

    Int operator/(const Int &rhs) const
    {
        if (abs(*this) < abs(rhs))
            return 0;
        Int res, rod = this->copy(rhs.len - 1);
        res.flag = flag ^ rhs.flag;
        res.len = len - rhs.len + 1;
        for (int i = res.len - 1; i >= 0; i--)
        {
            rod.shl();
            rod.v[0] = this->v[i];
            res.v[i] = Find(rhs, rod);
            rod = rod - rhs * res.v[i];
        }
        res.remove();
        return res;
    }

    Int operator%(const Int &rhs) const //only postive
    {
        if (abs(*this) < abs(rhs))
            return *this;
        Int res, rod = this->copy(rhs.len - 1);
        res.flag = flag ^ rhs.flag;
        res.len = len - rhs.len + 1;
        for (int i = res.len - 1; i >= 0; i--)
        {
            rod.shl();
            rod.v[0] = this->v[i];
            res.v[i] = Find(rhs, rod);
            rod = rod - rhs * res.v[i];
        }
        res.remove();
        return rod;
    }

    explicit operator ull()
    {
        ull res = 0;
        for (int i = len - 1; i >= 0; i--)
            res = res * base + (ull)v[i];
        return res;
    }

    void print() const;
    static Int Pow(const Int &a, const Int &x, const Int &rod)
    {
        Int b = a, ans = 1;
        for (int i = 0; i < x.len; i++)
        {
            ull k = x.v[i];
            for (int j = 1; j <= sizeof(unint) * 8; j++, k >>= 1, b = (b * b) % rod)
                if (k & 1)
                    ans = (ans * b) % rod;
        }
        return ans;
    }
};

void Int::print() const
{
    if (len <= 0)
    {
        printf("error %d\n", len);
        return;
    }

    ull buf[100];
    memset(buf, 0, sizeof(buf));
    int cnt = 0;
    buf[cnt++] = 0;
    for (int i = len - 1; i >= 0; i--)
    {
        for (int j = 0; j < cnt; j++)
            buf[j] *= base;
        buf[0] += v[i];
        cnt += 10;
        for (int j = 0; j < cnt; j++)
        {
            buf[j + 1] += buf[j] / 10;
            buf[j] %= 10;
        }
        while (cnt > 1 && buf[cnt - 1] == 0)
            cnt--;
    }

    printf("decimal Len: %d\n", cnt);
    if (flag)
        printf("-");
    for (int i = cnt - 1; i >= 0; i--)
        printf("%lld", buf[i]);
    printf("\n");

    printf("orginal Len:%d\n", len);
    if (flag)
        printf("-");
    for (int i = len - 1; i >= 0; i--)
        printf("%lld ", v[i]);
    printf("\n");
    
    /*
    printf("Len: %d\n", len);
    if (flag)
        printf("-");
    printf("%lld", v[len - 1]);
    for (int i = len - 2; i >= 0; i--)
        printf("%09lld", v[i]);
    printf("\n");
    */
}

#endif

/*
int main()
{
    ll a, b;
    // scanf("%lld %lld", &a, &b);
    // Int A = a, B = b;
    // (A + Int::rev(B)).print();
    Int::Pow(1234567895657345, 3457689980532535, 78659600356657582).print();
    return 0;
}
*/
