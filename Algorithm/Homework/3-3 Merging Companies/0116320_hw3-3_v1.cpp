#include<iostream>
#include<queue>
#include<vector>
using namespace std;
struct compare
{
  bool operator()(const int& l, const int& r)
  {
      return l > r;
  }
};
int main()
{
    int T,N,i,j,answer,temp,merge;
    priority_queue<int,vector<int>,compare> company;
    cin>>T;
    for(i=0;i<T;i++)
    {
        cin>>N;
        for(j=0;j<N;j++)
        {
            cin>>temp;
            company.push(temp);
        }
        answer=0;
        while(company.size()!=1)
        {
            merge=company.top();
            company.pop();
            merge+=company.top();
            company.pop();
            company.push(merge);
            answer+=merge;
        }
        cout<<answer<<endl;
        company.pop();
    }
    return 0;
}
