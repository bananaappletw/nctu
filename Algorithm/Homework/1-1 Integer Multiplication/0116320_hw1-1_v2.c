#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define max 10000000

long long ten[8]={1,10,100,1000,10000,100000,1000000,max};
int main()
{
    long long A[3000],B[3000],C[6000],carry;
    char a[20001],b[20001];
    int N,i,j,k,l,m,n,alength,blength,asize,bsize,csize,cbit,cbeginbit,cbegin,cend,clength,cendbit;
    scanf("%d",&N);
    for(k=0;k<N;k++)
    {
        scanf("%s",a);
        scanf("%s",b);
        scanf("%d%d",&i,&j);
        alength=strlen(a);
        blength=strlen(b);
        asize=(alength-1)/7+1;
        bsize=(blength-1)/7+1;
        /*
        printf("asize:%d",asize);
        printf("\n");
        printf("bsize:%d",bsize);
        printf("\n");
        */
        for(n=0,l=alength-1;n<asize;n++)
        {
            A[n]=0;
            for(m=0;m<7&&l>=0;m++,l--)
            {
                A[n]+=(a[l]-'0')*ten[m];
            }
        }
        A[asize]=0;
        for(n=0,l=blength-1;n<bsize;n++)
        {
            B[n]=0;
            for(m=0;m<7&&l>=0;m++,l--)
            {
                B[n]+=(b[l]-'0')*ten[m];
            }
        }
        B[bsize]=0;
        carry=0;
        for(n=0;n<asize+bsize;n++)
        {
            C[n]=carry;
            for(l=0;l<n+1;l++)
            {
                if(l<asize&&n-l<bsize)
                {
                    C[n]+=A[l]*B[n-l];
                }
            }
            if(C[n]>=max)
            {
                carry=C[n]/max;
                C[n]%=max;
            }
            else
                carry=0;
        }
        if(C[asize+bsize-1])
            csize=asize+bsize;
        else
            csize=asize+bsize-1;
        /*
        for(l=csize-1;l>=0;l--)
        {
            printf("%07lld",C[l]);
            printf(",");
        }
        printf("\n");
        */
        cbit=1;
        while(C[csize-1]/ten[cbit])
            cbit++;
        n=0;
        while(cbit+n*7<i)
            n++;
        cbegin=csize-1-n;
        cbeginbit=cbit+n*7-i+1;
        while(cbit+n*7<j)
            n++;
        cend=csize-1-n;
        cendbit=cbit+n*7-j+1;
        clength=cbegin-cend+1;
        if(clength>1)
        {
            printf("%0*lld",cbeginbit,C[cbegin]%ten[cbeginbit]);
            for(l=cbegin-1;l>cend;l--)
            {
                 printf("%0*lld",7,C[l]);
            }
            printf("%0*lld",7-cendbit+1,C[cend]/ten[cendbit-1]);
        }
        else
        {
             printf("%0*lld",cendbit-cbeginbit+1,((C[cbegin]%ten[cbeginbit])/ten[cendbit-1]));
        }
        printf("\n");
    }

}