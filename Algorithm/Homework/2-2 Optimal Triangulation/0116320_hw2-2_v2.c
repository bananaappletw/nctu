#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<float.h>
int x[200],y[200];
double vertax(int a, int b)
{
    double c,d,e;
    c=x[b]-x[a];
    d=y[b]-y[a];
    c*=c;
    d*=d;
    e=c+d;
    return  sqrt(e);
}
int main()
{
    int N,n,z,i,j,k;
    double length[200][200],min,temp;
    scanf("%d",&N);
    for(z=0;z<N;z++)
    {
        scanf("%d",&n);
        for(j=0;j<n;j++)
        {
            scanf("%d%d",&x[j],&y[j]);
        }
        for(i=0;i<n;i++)
            length[i][i]=0;
        for(i=2;i<n;i++)
        {
            for(j=0;j<n-i;j++)
            {
                min=DBL_MAX;
                for(k=j+1;k<j+i;k++)
                {
                    temp=length[j][k]+length[k][j+i];
                    if(temp<min)
                        min=temp;
                }
                length[j][j+i]=vertax(j,j+i)+min;
            }
        }
        printf("%g\n",length[0][n-1]-vertax(0,n-1));
    }
    return 0;
}

