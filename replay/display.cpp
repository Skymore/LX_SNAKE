#include"stdio.h"
#include"stdlib.h"
#include"string.h"
#define CHAR1 '1'
#define CHAR2 '3'
#define CHARFOOD '2'
#define CHARNULL '0'
#define LEN 10000
#define TIME 100
int main()
{
    FILE *fp=fopen("record.txt","r");
    int siz,sgn1=1,sgn2=1;
    while(fscanf(fp,"%d\n",&siz)!=EOF)
    {
        char s[LEN];
        for(int i=0; i<siz; i++)
        {
            fscanf(fp,"%s",s);
            for(int j=0; j<siz; j++)
            {
                switch(s[j])
                {
                case CHAR1:
                    printf("()");
                    break;
                case CHAR2:
                    printf("[]");
                    break;
                case CHARFOOD:
                    printf("::");
                    break;
                case CHARNULL:
                    printf("  ");
                    break;
                }
            }
            printf("#\n");
        }
        for(int j=0; j<siz; j++)
            printf("##");
        printf("#\n");
        printf("Step %d: Snake %d is thinking. \n",sgn1++,sgn2);
        switch(sgn2)
        {
        case 0:
            break;
        case 1:
            sgn2=2;
            break;
        case 2:
            sgn2=1;
            break;
        }
        char ch=getchar();
        if(ch!='\n')
        {
            getchar();
            int cnt=0,tmp=siz;
            if(tmp==0)
                cnt++;
            else
            {
                while(tmp)
                {
                    cnt++;
                    tmp/=10;
                }
            }
            fseek(fp,-2*(siz*(siz+2)+cnt+4),1);
            sgn1-=2;
        }
        system("cls");
    }
    fclose(fp);
    return 0;
}
