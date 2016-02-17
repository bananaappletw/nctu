#include<stdio.h>
#include<stdlib.h>
int main()
{
    int T,n,i,j,s,f,answer,correnttime,correntactivity;
    int  activity[100000];
    scanf("%d",&T);
    for(i=0;i<T;i++)
    {
        scanf("%d",&n);
        for(j=0;j<100000;j++)
            activity[j]=-1;
        for(j=0;j<n;j++)
        {
            scanf("%d",&s);
            scanf("%d",&f);
            if(s>activity[f])
                activity[f]=s;
        }
        correnttime=0;
        answer=0;
       for(correntactivity=1;correntactivity<100000;correntactivity++)
        {
            if(activity[correntactivity]>=correnttime)
            {
                answer++;
                correnttime=correntactivity;
            }
        }
        printf("%d\n",answer);
    }
    return 0;
}
