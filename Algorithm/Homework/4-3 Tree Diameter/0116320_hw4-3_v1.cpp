#include<iostream>
#include<cstdio>
#include<list>
using namespace std;
bool visited[10001];
int N,i,n,m,j,a,b,answer,first;
list<int>* vertax=new list<int>[10001];
void dfs(int point,int length)
{
    visited[point]=true;
    if(length>answer)
    {
        answer=length;
        first=point;
    }
    for(list<int>::iterator it=vertax[point].begin();it!=vertax[point].end();it++)
    {
        if (!visited[*it])
        {
            dfs(*it,length+1);
        }
    }
}
int main()
{
    scanf("%d",&N);
    for(i=0;i<N;i++)
    {
        answer=0;
        first=0;
        scanf("%d",&n);
        for(j=1;j<=n;j++)
        {
            vertax[j].clear();
            visited[j]=false;
        }
        for(j=0;j<n-1;j++)
        {
            scanf("%d %d",&a,&b);
            vertax[a].push_back(b);
            vertax[b].push_back(a);
        }
        dfs(1,0);
        answer=0;
        for(j=1;j<=n;j++)
        {
            visited[j]=false;
        }
        dfs(first,0);
        printf("%d\n",answer);
    }
    return 0;
}
