#include<stdio.h>
#include<stdlib.h>
int vertax[5001];
int size[5001];
int find(int x)
{
    return (x==vertax[x])?x:(vertax[x]=find(vertax[x]));
}
int merge(int x,int y)
{
    if(find(x)==find(y))
        return 0;
    else
    {
        if(size[find(x)]<size[find(y)])
        {
            size[find(y)]+=size[find(x)];
            size[find(x)]=0;
            vertax[find(x)]=find(y);
        }
        else
        {
            size[find(x)]+=size[find(y)];
            size[find(y)]=0;
            vertax[find(y)]=find(x);
        }
        return 1;
    }
}
int main()
{
    int N,i,n,m,j,answer,a,b;
    scanf("%d",&N);
    for(i=0;i<N;i++)
    {
        answer=0;
        scanf("%d %d",&n,&m);
        for(j=1;j<=n;j++)
        {
            vertax[j]=j;
            size[j]=1;
        }
        for(j=0;j<m;j++)
        {
            scanf("%d %d",&a,&b);
            merge(a,b);
        }
        for(j=1;j<=n;j++)
            if(size[j])
                answer++;
        printf("%d\n",answer);
    }
    return 0;
}
