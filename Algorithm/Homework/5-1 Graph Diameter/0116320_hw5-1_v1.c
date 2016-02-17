#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
int distance[301][301];
int main()
{
    int N,h,i,j,k,l,n,m,a,b,c,answer;
    scanf("%d",&N);
    for(h=0;h<N;h++)
    {

        scanf("%d %d",&n,&m);
        for (i = 1; i <= n; i++)
        {
            for (j = 1; j <= n; j++)
            {
                distance[i][j]=INT_MAX;
            }
        }
        for(i=1;i<=n;i++)
            distance[i][i]=0;
        for(i=0;i<m;i++)
        {
            scanf("%d %d %d",&a,&b,&c);
            if(a<b)
            {
                if(distance[a][b]>c)
                distance[a][b]=c;
            }
            else
            {
                if(distance[b][a]>c)
                    distance[b][a]=c;
            }
        }
        for(k=1;k<=n;k++)
            for(i=1;i<=n;i++)
                for(j=i;j<=n;j++)
                {
                    if(i<k)
                        a=distance[i][k];
                    else
                        a=distance[k][i];
                    if(k<j)
                        b=distance[k][j];
                    else
                        b=distance[j][k];
                    if (a!=INT_MAX&&b!=INT_MAX &&a+b<distance[i][j])
                            distance[i][j]=a+b;
                }
        answer=0;
        for(i=1;i<=n;i++)
            for(j=i;j<=n;j++)
            {
                if (distance[i][j]>answer)
                    answer=distance[i][j];
            }
        if(answer==INT_MAX)
            printf("Infinity\n");
        else
            printf("%d\n",answer);
    }
    return 0;
}
