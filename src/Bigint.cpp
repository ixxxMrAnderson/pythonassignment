//
// Created by chtvii on 12/9/19.
//

#include "Bigint.h"
int lx,ly,a[100010],b[100010],c[100010],d[100010];
bool Bjudge(const Bigint &x,const Bigint &y)
{
    if (x.pst == 1 && y.pst == 0) return 1;
    if (x.pst == 0 && y.pst == 1) return 0;
    if (x.pst == 1 && y.pst == 1)
    {
        if (x.num.size() > y.num.size()) return 1;
        if (x.num.size() < y.num.size()) return 0;
        for (int i = 0; i < x.num.size();++i)
        {
            if (x.num[i] > y.num[i]) return 1;
            if (x.num[i] < y.num[i]) return 0;
        }
        return 1;
    }
    if (x.pst == 0 && y.pst == 0)
    {
        if (x.num.size() > y.num.size()) return 0;
        if (x.num.size() < y.num.size()) return 1;
        for (int i = 0; i < x.num.size();++i)
        {
            if (x.num[i] > y.num[i]) return 0;
            if (x.num[i] < y.num[i]) return 1;
        }
        return 1;
    }
}
Bigint Badd(Bigint x,Bigint y)
{
    memset(a,0,sizeof(a));
    memset(b,0,sizeof(b));
    memset(c,0,sizeof(c));
    Bigint tmpB;
    tmpB.pst = 1;
    if (x.num[0] == 0 && y.num[0] == 0)
    {
        return tmpB;
    }
    tmpB.num.clear();
    if (x.pst == 0 && y.pst == 1)
    {
        x.pst = 1;
        return Bsubtract(y,x);
    }
    if (x.pst == 1 && y.pst == 0)
    {
        y.pst = 1;
        return Bsubtract(x,y);
    }
    if (x.pst == 0 && y.pst == 0) tmpB.pst = 0;
    lx = x.num.size();
    ly = y.num.size();
    for(int i = lx - 1;i >= 0; --i) a[lx - i - 1] = x.num[i];
    for(int i = ly - 1;i >= 0; --i) b[ly - i - 1] = y.num[i];
    for(int i = 0;i < max(lx,ly); ++i)
    {
        c[i] += a[i] + b[i];
        if (c[i] >= 10)
        {
            c[i] = c[i] % 10;
            c[i + 1] += 1;
        }
    }
    int t = 1;
    for (long int i = 100005;i >= 0; --i)
    {
        if (c[i] != 0 || t == 0)
        {
            tmpB.num.push_back(c[i]);
            t = 0;
        }
    }
    return tmpB;
}
Bigint Bsubtract(Bigint x,Bigint y)
{
    memset(a,0,sizeof(a));
    memset(b,0,sizeof(b));
    memset(c,0,sizeof(c));
    Bigint tmpB;
    tmpB.pst = 1;
    if (x.pst == y.pst && Bjudge(x,y) && Bjudge(y,x))
    {
        return tmpB;
    }
    tmpB.num.clear();
    if (!Bjudge(x,y))
    {
        tmpB = Bsubtract(y,x);
        tmpB.pst = 0;
        return tmpB;
    }
    if (x.pst == 0 && y.pst == 0)
    {
        x.pst = 1;
        y.pst = 1;
        return Bsubtract(y,x);
    }
    if (x.pst == 1 && y.pst == 0)
    {
        y.pst = 1;
        return Badd(x,y);
    }
    lx = x.num.size();
    ly = y.num.size();
    for(int i = lx - 1;i >= 0; --i) a[lx - i - 1] = x.num[i];
    for(int i = ly - 1;i >= 0; --i) b[ly - i - 1] = y.num[i];
    for(int i = 0;i < max(lx,ly); ++i)
    {
        c[i] += a[i] - b[i];
        if (c[i] < 0)
        {
            c[i] += 10;
            c[i + 1] -= 1;
        }
    }
    int t = 1;
    for (long int i = 100005;i >= 0; --i)
    {
        if (c[i] != 0 || t == 0)
        {
            tmpB.num.push_back(c[i]);
            t = 0;
        }
    }
    return tmpB;
}
Bigint Bmultiply(Bigint x,Bigint y)
{
    memset(a,0,sizeof(a));
    memset(b,0,sizeof(b));
    memset(c,0,sizeof(c));
    Bigint tmpB;
    tmpB.pst = 1;
    if (x.num[0] == 0 || y.num[0] == 0)
    {
        return tmpB;
    }
    tmpB.num.clear();
    if (x.pst == 0 && y.pst == 1) tmpB.pst = 0;
    if (x.pst == 1 && y.pst == 0) tmpB.pst = 0;
    lx = x.num.size();
    ly = y.num.size();
    for(int i = lx - 1;i >= 0; --i) a[lx - i - 1] = x.num[i];
    for(int i = ly - 1;i >= 0; --i) b[ly - i - 1] = y.num[i];
    for(int i = 0;i < lx; ++i)
    {
        for(int j = 0;j < ly; ++j)
        {
            c[i + j ] += a[i] * b[j];
            if (c[i + j] >= 10)
            {
                c[i + j + 1] += c[i + j] / 10;
                c[i + j] %= 10;
            }
        }
    }
    int t = 1;
    for (long int i = 100005;i >= 0; --i)
    {
        if (c[i] != 0 || t == 0)
        {
            tmpB.num.push_back(c[i]);
            t = 0;
        }
    }
    return tmpB;
}
Bigint Bdivide(Bigint x,Bigint y)
{
    memset(d,0,sizeof(d));
    bool jdd;
    Bigint tmpB;
    tmpB.pst = 1;
    if (x.num[0] == 0 || x.pst == y.pst && !Bjudge(x,y))
    {
        return tmpB;
    }
    if (x.pst != y.pst)
    {
        tmpB.pst = 0;
        x.pst = 1;
        y.pst = 1;
        if (!Bjudge(x,y))
        {
            tmpB.num.push_back(1);
            return tmpB;
        }
    }
    x.pst = 1;
    y.pst = 1;
    tmpB.num.clear();
    ly = y.num.size();
    Bigint tmpd;
    tmpd.pst = 1;
    while(Bjudge(x,y))
    {
        jdd = 0;
        tmpd.num.clear();
        for (int i = 0; i < ly;++i) tmpd.num.push_back(x.num[i]);
        if (!Bjudge(tmpd,y)) {tmpd.num.push_back(x.num[ly]);jdd = 1;}
        int cntd = 0,lt = tmpd.num.size();
//for (int i = 0; i < lt;++i)cout << tmpd.num[i];
//cout << endl;
        while(Bjudge(tmpd,y))
        {
            tmpd = Bsubtract(tmpd,y);
            cntd ++;
        }
//cout <<x.num.size()<<endl;
        if (cntd != 0) d[x.num.size() - lt] = cntd;
        for (int i = ly + jdd; i < x.num.size();++i) tmpd.num.push_back(x.num[i]);
        x.num.clear();
        jdd = 1;
//for (int i = 0; i < 10;++i)cout << d[i];
//cout << endl;
        for (int i = 0; i < tmpd.num.size();++i)
        {
            if (tmpd.num[i] != 0) jdd = 0;
            if (jdd)
            {
                continue;
            }
            x.num.push_back(tmpd.num[i]);
        }
    }
    int t = 1;
    for (long int i = 100005;i >= 0; --i)
    {
        if (d[i] != 0 || t == 0)
        {
            tmpB.num.push_back(d[i]);
            t = 0;
        }
    }
    if (tmpB.pst == 0 && !(tmpB * y == x)){
        tmpB = tmpB - Bigint(1);
    }
    return tmpB;
}
Bigint Bmod(const Bigint &x,const Bigint &y) {return (Bsubtract(x,Bmultiply(y,Bdivide(x,y))));}

