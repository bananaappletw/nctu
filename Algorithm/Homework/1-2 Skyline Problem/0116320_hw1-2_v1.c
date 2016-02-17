#include<stdio.h>
#include<stdlib.h>
int l[20000],h[20000],r[20000];
int merge(int* x,int* height,int begin,int end){
	if(end-begin==1)
    {
		x[0]=l[begin];
		height[0]=r[begin];
		x[1]=h[begin];
		height[1]=0;
		return 2;
	}
	int middle=(begin+end)/2;
	int *leftx,*leftheight,*rightx,*rightheight;
	int leftlength,rightlength,left=0,right=0,length=0,nextheight,nextx,pastheight=0;
	leftx=malloc(2*sizeof(int)*(middle-begin));
	leftheight=malloc(2*sizeof(int)*(middle-begin));
	rightx=malloc(2*sizeof(int)*(end-middle));
	rightheight=malloc(2*sizeof(int)*(end-middle));
	leftlength=merge(leftx,leftheight,begin,middle);
	rightlength=merge(rightx,rightheight,middle,end);
	while(left<leftlength&&right<rightlength)
	{
		if(leftx[left]<rightx[right])
        {
			nextx=leftx[left];
			if(right==0)
                nextheight=leftheight[left];
			else
			{
			    if(leftheight[left]>rightheight[right-1])
                    nextheight=leftheight[left];
                else
                    nextheight=rightheight[right-1];
			}
			left++;
        }
		else if(leftx[left]==rightx[right])
        {

            nextx=leftx[left];
            if(leftheight[left]>rightheight[right])
                nextheight=leftheight[left];
			else
                nextheight=rightheight[right];
			left++,right++;
		}
		else
        {
			nextx=rightx[right];
			if(left==0)
                nextheight=rightheight[right];
			else
            {
                if(rightheight[right]>leftheight[left-1])
                    nextheight=rightheight[right];
                else
                    nextheight=leftheight[left-1];
            }
			right++;
		}
		if(nextheight!=pastheight)
        {
			pastheight=nextheight;
			x[length]=nextx;
			height[length]=nextheight;
			length++;
		}
    }
	while(left<leftlength)
    {
		if(leftheight[left]!=height[length-1])
		{
			height[length]=leftheight[left];
			x[length]=leftx[left];
			length++;
		}
		left++;
    }
	while(right<rightlength)
	{
		if(rightheight[right]!=height[length-1])
		{
			height[length]=rightheight[right];
			x[length]=rightx[right];
			length++;
		}
		right++;
    }
    free (leftx);
	free (leftheight);
	free (rightx);
	free (rightheight);
	return length;
}

int main()
{
	int answerx[20000],answerh[20000],build,i,j,N,T;
	scanf("%d",&N);
	for(j=0;j<N;j++)
    {
        scanf("%d",&T);
        for(i=0;i<T;i++)
            scanf("%d%d%d",&l[i],&r[i],&h[i]);
        build=merge(answerx,answerh,0,i);
        for(i=0;i<build;i++)
        {
            printf("%d %d",answerx[i],answerh[i]);
            if(i!=build-1)
                printf(" ");
            else
                printf("\n");
        }
    }
	return 0;
}
