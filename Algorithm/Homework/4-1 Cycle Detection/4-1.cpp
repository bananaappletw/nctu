#include<iostream>
#include<cstdio>
#include<list>
using namespace std;
bool used[5001],visited[5001],answer;
int N,i,n,m,j,a,b;
list<int>* vertax=new list<int>[5001];
bool dfs(int point)
{
    if(!visited[point])
    {
        visited[point]=true;
        used[point]=true;
        for(list<int>::iterator it=vertax[point].begin();it!=vertax[point].end();it++)
        {
            if (!visited[*it]&&dfs(*it))
                return true;
            else if(used[*it])
                return true;
        }
    }
    used[point]=false;
    return false;
}
int main()
{
    scanf("%d",&N);
    for(i=0;i<N;i++)
    {
        answer=false;
        scanf("%d%d",&n,&m);
        for(j=1;j<=n;j++)
        {
            vertax[j].clear();
            used[j]=visited[j]=false;
        }
        for(j=0;j<m;j++)
        {
            scanf("%d %d",&a,&b);
            vertax[a].push_back(b);
        }
        for(j=1;j<=n;j++)
        {
            if(dfs(j))
            {
                answer=true;
                break;
            }
        }
        if(answer)
            printf("YES\n");
        else
            printf("NO\n");
    }
    return 0;
}
