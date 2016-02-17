#include<stdio.h>
#include<stdlib.h>
struct highway
{
    int v;
    int u;
    int w;
};
typedef struct highway Highway;
int towns[10001];
Highway edge[200000];
Highway temp[200000];
int count[1001];
int size[10001];
int find(int x)
{
    return (x==towns[x])?x:(towns[x]=find(towns[x]));
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
            towns[find(x)]=find(y);
        }
        else
        {
            size[find(x)]+=size[find(y)];
            size[find(y)]=0;
            towns[find(y)]=find(x);
        }
        return 1;
    }
}
int main()
{
    int T,n,m,i,j,answer,connect;

    scanf("%d",&T);
    for(i=0;i<T;i++)
    {
        scanf("%d",&n);
        scanf("%d",&m);
        for(j=1;j<=n;j++)
        {
            towns[j]=j;
            size[j]=1;
        }
        for(j=0;j<1001;j++)
        {
            count[j]=0;
        }
        for(j=0;j<m;j++)
        {
            scanf("%d %d %d",&edge[j].v,&edge[j].u,&edge[j].w);
            count[edge[j].w]++;
        }
        for(j=1;j<1001;j++)
        {
            count[j]+=count[j-1];
        }
        for(j=0;j<m;j++)
        {
            temp[--count[edge[j].w]]=edge[j];
        }
        for(j=0;j<m;j++)
        {
            edge[j]=temp[j];
        }
        answer=0;
        connect=0;
        for(j=0;j<m;j++)
        {
            if(merge(edge[j].v,edge[j].u))
            {
                answer+=edge[j].w;
                connect++;
            }
            if(connect==n-1)
                break;
        }
        printf("%d\n",answer);
    }
    return 0;
}
