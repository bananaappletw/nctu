#include<iostream>
#include<queue>
#include<list>
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<climits>
using namespace std;
struct node {int x, y;};
bool operator<(const struct node& n1, const struct node& n2)
{
    return n1.y > n2.y;
}
int d[3001];
list<struct node>* u=new list<struct node>[10001];
list<struct node>::iterator it;
bool visit[3001];
int main()
{
    int N,i,j,n,m,a,b,c,answer;
    scanf("%d",&N);
    for(i=0;i<N;i++)
    {
        priority_queue<struct node> vertax;
        scanf("%d %d",&n,&m);
        for(j=1;j<=n;j++)
        {
            d[j]=INT_MAX;
            visit[j]=false;
            u[j].clear();
        }
        d[1]=0;
        for(j=0;j<m;j++)
        {
            scanf("%d %d %d",&a,&b,&c);
            u[a].push_back((struct node){b,c});
            u[b].push_back((struct node){a,c});
        }
        vertax.push((struct node){1, d[1]});
        for(j=0; j<n; j++)
        {
            while(visit[a=vertax.top().x])
                    vertax.pop();
            visit[a] = true;
            for(it=u[a].begin();it!=u[a].end();it++)
            {
                if (!visit[it->x])
                {
                    if(d[a]+it->y<d[it->x])
                    {
                        d[it->x]=d[a]+it->y;
                        vertax.push((struct node){it->x,d[it->x]});
                    }
                }
            }
        }

        answer=0;
        for(j=2;j<=n;j++)
        {
            if(d[j]>answer)
                answer=d[j];
        }
        printf("%d\n",answer);
    }
    return 0;
}
