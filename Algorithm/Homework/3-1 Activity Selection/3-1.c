#include<stdio.h>
#include<stdlib.h>
struct activity
{
    int s;
    int f;
};
typedef struct activity Activity;
int compare(const void* a,const void* b)
{
    if((*(Activity*)a).f<(*(Activity*)b).f)
        return -1;
    else if((*(Activity*)a).f>(*(Activity*)b).f)
        return 1;
    else
        return 0;
}
int main()
{
    int T,n,i,j,answer,temp,merge,correnttime,correntactivity;
    Activity all[400000];
    scanf("%d",&T);
    for(i=0;i<T;i++)
    {
        scanf("%d",&n);
        for(j=0;j<n;j++)
        {
            scanf("%d",&all[j].s);
            scanf("%d",&all[j].f);
        }
        qsort(all,n,sizeof(Activity),compare);
        /*for(j=0;j<n;j++)
            printf("%d %d\n",all[j].s,all[j].f);*/
        correnttime=all[0].f;
        answer=1;
       for(correntactivity=1;correntactivity<n;correntactivity++)
        {
            if(all[correntactivity].s>=correnttime)
            {
                correnttime=all[correntactivity].f;
                answer++;
            }
        }
        printf("%d\n",answer);
    }
    return 0;
}
