#include<iostream>
#include <iomanip>
#include<cstdio>
#include<cstring>
#define max 1000000000000000000
using namespace std;
int N,n,i,j,k,mergelength;
unsigned long long carry;
class BigNum
{
public:
    BigNum():length(1)
    {
        data[0]=0;
        data[1]=1;
        data[2]=0;
        data[3]=0;
        data[4]=0;
        data[5]=0;
    }
    void operator+=(const BigNum &add)
    {
        carry=0;
        for(i=1;i<=mergelength;i++)
        {
            data[i]+=add.data[i]+carry;
            if(data[i]>=max)
            {
                carry=data[i]/max;
                data[i]%=max;
            }
            else
                carry=0;
        }
        data[1]+=add.data[1];
        data[2]+=add.data[2];
        data[3]+=add.data[3];
        data[4]+=add.data[4];
        data[5]+=add.data[5];
        if (data[1] >= max)
        {
            data[2] += data[1]/max;
            data[1] %= max;
        }
        if (data[2] >= max)
        {
            data[3] += data[2]/max;
            data[2] %= max;
        }
        if (data[3] >= max)
        {
            data[4] += data[3]/max;
            data[3] %= max;
        }
        if (data[4] >= max)
        {
            data[5] += data[4]/max;
            data[4] %= max;
        }
        if(data[2])
            length=2;
        if(data[3])
            length=3;
        if(data[4])
            length=4;
        if(data[5])
            length=5;

    }
    friend ostream &operator<<(ostream &, const BigNum &);
//private:
        unsigned long long data[6];
        int length;
};
ostream &operator<<(ostream & output, const BigNum & value)
{
    output<<value.data[value.length];
    for(i=value.length-1;i>0;i--)
        output<<setfill('0')<<setw(18)<<value.data[i];
    return output;
}
int main()
{

    BigNum Rod[5001];
    for(j=1;j<5000;j++)
    {
        for(k=j+1;k<=5000;k++)
        {
            Rod[k]+=Rod[k-j-1];
        }
    }
    scanf("%d",&N);
    for(j=0;j<N;j++)
    {
        scanf("%d",&n);
        cout<<Rod[n]<<endl;
    }
    return 0;
}

