#include<iostream>
#include<queue>
#include<list>
#include<cstdio>
#include<climits>
#include<queue>
using namespace std;
struct edge
{
    int to,residue;
};
list<struct edge>* vertax=new list<struct edge>[50];
int N,n,m,s,t,u,v,c,i,j,f;
int d[50];
int visit[50];
int answer;
int shortest_path()
{
    for(i=0;i<n;i++)
    {
        visit[i]=0;
        d[i]=INT_MAX;
    }
    d[s]=0;
    visit[s]=1;
    queue<int> q;
    q.push(s);
    while(!q.empty())
    {
        i=q.front();
        q.pop();
        for(list<struct edge>::iterator it=vertax[i].begin();it!=vertax[i].end();it++)
        {
            if(!visit[it->to]&&it->residue>0)
            {
                d[it->to]=d[i]+1;
                visit[it->to]=1;
                q.push(it->to);
                if(it->to==t)
                    return 1;
            }
        }
    }
    return 0;
}
int path(int point,int flow)
{
    if(point==t)
        return flow;
    if(visit[point])
        return 0;
    visit[point]=1;
    for(list<struct edge>::iterator it=vertax[point].begin();it!=vertax[point].end();it++)
    {
        if(it->residue>0&&d[it->to]==d[point]+1)
        {
            f=path(it->to,min(it->residue,flow));
            if(f)
            {
                it->residue-=f;
                for(list<struct edge>::iterator that=vertax[it->to].begin();that!=vertax[it->to].end();that++)
                    if(that->to==point)
                        that->residue+=f;
                return f;
            }
        }
    }
    return 0;
}
int main()
{
    scanf("%d",&N);
    for(j=0;j<N;j++)
    {
        scanf("%d %d %d %d",&n,&m,&s,&t);
        answer=0;
        for(i=0;i<n;i++)
        {
            vertax[i].clear();
        }
        for(i=0;i<m;i++)
        {
            scanf("%d %d %d",&u,&v,&c);
            vertax[u].push_back((struct edge){v,c});
            vertax[v].push_back((struct edge){u,0});
        }
        shortest_path();
        while(shortest_path())
        {
            while(true)
            {
                for(i=0;i<n;i++)
                {
                    visit[i]=0;
                }
                f=path(s,INT_MAX);
                if(!f)
                    break;
                answer+=f;
            }
        }
        printf("%d\n",answer);
    }
    return 0;
}
