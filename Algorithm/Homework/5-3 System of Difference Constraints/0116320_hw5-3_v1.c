#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
struct node {int x, y, z;};
struct node graph[5001];
int d[1001];
int main()
{
    int N,i,j,k,n,m,answer;
    scanf("%d",&N);
    for(i=0;i<N;i++)
    {
        scanf("%d %d",&n,&m);
        answer=1;
        for(j=2;j<=n;j++)
           d[j]=INT_MAX;
        d[1]=0;
        for(j=0;j<m;j++)
        {
           scanf("%d %d %d",&graph[j].y,&graph[j].x,&graph[j].z);
        }
        for(j=0; j<n-1; j++)
            for(k=0;k<m;k++)
                if(d[graph[k].x]!=INT_MAX)
                    if(d[graph[k].x]+graph[k].z<d[graph[k].y])
                       d[graph[k].y]=d[graph[k].x]+graph[k].z;

        for(j=0;j<m;j++)
            if(d[graph[j].x]!=INT_MAX)
                if(d[graph[j].x]+graph[j].z<d[graph[j].y])
                    answer=0;
        if(answer)
            printf("Yes\n");
        else
            printf("No\n");
    }
    return 0;
}
