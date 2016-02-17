#include<stdio.h>
#include<stdlib.h>
struct highway
{
    int v;
    int u;
    int w;
};
typedef struct highway Highway;
int compare(const void* a,const void* b)
{
    if((*(Highway*)a).w<(*(Highway*)b).w)
        return -1;
    else if((*(Highway*)a).w>(*(Highway*)b).w)
        return 1;
    else
        return 0;
}
int towns[10001];
Highway edge[200000];
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
        for(j=0;j<m;j++)
        {
            scanf("%d",&edge[j].v);
            scanf("%d",&edge[j].u);
            scanf("%d",&edge[j].w);
        }
        qsort(edge,m,sizeof(Highway),compare);
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
