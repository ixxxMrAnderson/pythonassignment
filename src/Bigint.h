//
// Created by chtvii on 12/9/19.
//

#ifndef PYTHON_INTERPRETER_BIGINT_H
#define PYTHON_INTERPRETER_BIGINT_H


class Bigint;
class Nonetype{};
#include <algorithm>
#include <vector>
#include <cstring>
#include <iostream>
#include <cmath>

using namespace std;

class Bigint{
    friend bool Bjudge(const Bigint &x,const Bigint &y);
    friend bool operator==(const Bigint &x,const Bigint &y)
    {
        if (x.num.size() != y.num.size()) return 0;
        else
        {
            for (int i = 0; i < x.num.size();++i)
            {
                if (x.num[i] != y.num[i]) return 0;
            }
        }
        return 1;
    }
    friend Bigint Badd(Bigint x,Bigint y);
    friend Bigint operator+(Bigint x,Bigint y) {return Badd(x,y);}
    friend Bigint Bsubtract(Bigint x,Bigint y);
    friend Bigint operator-(Bigint x,Bigint y) {return Bsubtract(x,y);}
    friend Bigint Bmultiply(Bigint x,Bigint y);
    friend Bigint operator*(Bigint x,Bigint y) {return Bmultiply(x,y);}
    friend Bigint Bdivide(Bigint x,Bigint y);
    friend Bigint operator/(Bigint x,Bigint y) {return Bdivide(x,y);}
    friend Bigint Bmod(const Bigint &x,const Bigint &y);
    friend Bigint operator%(Bigint x,Bigint y) {return Bmod(x,y);}
private:
    vector<int> num;
    int pst;
public:
    Bigint(int x = 0)
    {
        //cout << "creatingBigint:"<< x << endl;
        num.clear();
        if (x < 0) {pst = 0;x*=(-1);}
        else pst = 1;
        int cntc = 0;
        int cB[10005]{0};
        if (x == 0) {
            num.push_back(0);
            return;
        }
        num.clear();
        while(x!= 0)
        {
            cB[cntc] = x%10;
            x/=10;
            cntc++;
        }
        int t = 1;
        for (long int i = 10000;i >= 0; --i)
        {
            if (cB[i] != 0 || t == 0)
            {
                num.push_back(cB[i]);
                t = 0;
            }
        }
        cout << "size:"<<num.size() << " num0:"<<num[0] << endl;
    }
    void createB(string s)
    {
        num.clear();
        if (s[0] == '-') pst = 0;
        else pst = 1;
        for (int i = 1 - pst; s[i] >= '0' && s[i] <= '9'; ++i) num.push_back(s[i] - '0');
    }
    void printB() const
    {
        if (pst == 0) cout << "-";
        for (int i = 0; i < num.size(); ++i) cout << num[i];
    }
    operator double() const
    {
        double Bdouble = 0;
        for (int i = num.size() - 1;i>=0;--i) {Bdouble += num[i]*pow(10,num.size() - i - 1);}
        return Bdouble;
    }
    operator bool() const
    {
        if (num[0] == 0) return 0;
        else return 1;
    }
    operator string() const
    {
        char Bstr[100005];
        for (int i = 0; i < num.size();++i)
        {
            Bstr[i] = num[i] + '0';
        }
        Bstr[num.size()] = '\0';
        return Bstr;
    }
    ~Bigint() {num.clear();}
};


#endif //PYTHON_INTERPRETER_BIGINT_H
