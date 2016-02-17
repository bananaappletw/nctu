#include<stdio.h>
#include<stdlib.h>
int maxheight;
int tree[20000];
void answer(int height,int begin,int end)
{
    //printf("%d:%d\n",begin,end);
    if(height>maxheight)
        maxheight=height;
    if(end-begin==1)
    {
        if(height+1>maxheight)
            maxheight=height+1;
    }
    if(begin==end)
    {
        return;
    }
    int index;
    int duplicate=0;
    int both=0;
    int left=begin+1;
    int right=end;
    int middle=(left+right)>>1;
    int x=tree[begin];
    while(1)
    {
        if(tree[middle-1]<x&&tree[middle]>x)
        {
            both=1;
            break;
        }
        if(left>=right)
        {
            duplicate=1;
            break;
        }
        if(tree[middle]<x)
        {
            left=middle+1;
            middle=(left+right)>>1;
        }
        else
        {
            right=middle-1;
            middle=(left+right)>>1;
        }
    }
    if(both)
    {
        answer(height+1,begin+1,middle-1);
        answer(height+1,middle,end);

    }
    else if(duplicate)
    {
        answer(height+1,begin+1,end);
    }
}
int main()
{
    int N=0,n,i,j,index;
    scanf("%d",&N);
    for(i=0;i<N;i++)
    {
        maxheight=0;
        scanf("%d",&n);
        for(j=0;j<n;j++)
        {
            scanf("%d",&tree[j]);
        }
        answer(0,0,n-1);
        printf("%d\n",maxheight);
        for(j=0;j<n;j++)
            tree[j]=0;
    }
    //system("PAUSE");
    return 0;
}
