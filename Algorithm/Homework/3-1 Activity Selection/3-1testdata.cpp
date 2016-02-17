#include<iostream>
#include<ctime>
#include<cstdlib>
using namespace std;
int main()
{
    int i,j,s,f,temp,n;
    int test,size,range;
    test=16;
    size=400000;
    range=100000;
    srand(time(NULL));
    cout<<test<<endl;
    for(i=0;i<test;i++)
    {
        n=rand()%size;
        cout<<n<<endl;
        for(j=0;j<n;j++)
        {
            s=rand()%(range-1);
            f=s+1+rand()%(range-s-1);
            cout<<s<<" "<<f<<endl;
        }
    }
    return 0;
}
