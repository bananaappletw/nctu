#include<stdio.h>
#include<stdlib.h>
int main()
{
	int N, i, k, j, q,size,middle,left,right;
	int a[20000];
	int sequence[20000];
	scanf("%d", &N);
	for (i = 0; i<N; i++)
	{
		scanf("%d", &k);
		for (j = 0; j<k; j++)
			scanf("%d",&a[j]);
		size = 1;
		sequence[0] = a[0];
		for (q = 1; q<k; q++)
		{
			if(sequence[size-1]<=a[q])
            {
                sequence[size]=a[q];
                size++;
            }
            else
            {
                left=0;
                right=size-1;
                while(left<right)
                {
                    middle=(left+right)>>1;
                    if(sequence[middle]<=a[q])
                    {
                        left=middle+1;
                    }
                    else
                    {
                        right=middle;
                    }
                }
                sequence[left]=a[q];
            }
		}
		printf("%d\n",size);
	}
	return 0;
}

