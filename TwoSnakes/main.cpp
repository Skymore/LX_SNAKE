
#include"main.h"
#include"stdio.h"
#include"stdlib.h"
#include"string.h"

#define D 2
#define L 3
#define R 1
#define U 0

#define LENDIR 4

#define MAXINT 2147483647

#define NUM1 1
#define NUM2 3
#define NUMFOOD 2
#define NUMNULL 0

bool liv1,liv2;
Data data;
int dirX[LENDIR],dirY[LENDIR];
Point dirP[LENDIR];

int Init_1(int *tmpData)
{
    dirX[U]=-1;
    dirY[U]=0;
    dirX[D]=1;
    dirY[D]=0;
    dirX[L]=0;
    dirY[L]=-1;
    dirX[R]=0;
    dirY[R]=1;
    for(int i=0;i<LENDIR;i++)
        dirP[i]=Point(dirX[i],dirY[i]);
    data.food.x=tmpData[0];
    data.food.y=tmpData[1];
    data.mSize=tmpData[4];
    liv1=false;
    liv2=false;
    for(int i=0; i<40; i++)
    {
        for(int j=0; j<40; j++)
        {
            data.m[i][j]=tmpData[40+i*40+j];
            switch(data.m[i][j])
            {
            case NUM1:
                liv1=true;
                break;
            case NUM2:
                liv2=true;
                break;
            }
        }
    }
    data.s1.tail=tmpData[2000];
    data.s1.head=tmpData[2002];
    data.s1.len=tmpData[2005];
    for(int i=0; i<76; i++)
    {
        data.s1.body[i].x=tmpData[2006+2*i];
        data.s1.body[i].y=tmpData[2006+2*i+1];
    }
    data.s2.tail=tmpData[3000];
    data.s2.head=tmpData[3002];
    data.s2.len=tmpData[3005];
    for(int i=0; i<76; i++)
    {
        data.s2.body[i].x=tmpData[3006+2*i];
        data.s2.body[i].y=tmpData[3006+2*i+1];
    }
    return 0;
}

int Init_2(int *tmpData)
{
    dirX[U]=-1;
    dirY[U]=0;
    dirX[D]=1;
    dirY[D]=0;
    dirX[L]=0;
    dirY[L]=-1;
    dirX[R]=0;
    dirY[R]=1;
    for(int i=0;i<LENDIR;i++)
        dirP[i]=Point(dirX[i],dirY[i]);
    data.food.x=tmpData[0];
    data.food.y=tmpData[1];
    data.mSize=tmpData[4];
    liv1=false;
    liv2=false;
    for(int i=0; i<40; i++)
    {
        for(int j=0; j<40; j++)
        {
            data.m[i][j]=tmpData[40+i*40+j];
            switch(data.m[i][j])
            {
            case NUM1:
                data.m[i][j]=NUM2;
                liv2=true;
                break;
            case NUM2:
                data.m[i][j]=NUM1;
                liv1=true;
                break;
            }
        }
    }
    data.s2.tail=tmpData[2000];
    data.s2.head=tmpData[2002];
    data.s2.len=tmpData[2005];
    for(int i=0; i<76; i++)
    {
        data.s2.body[i].x=tmpData[2006+2*i];
        data.s2.body[i].y=tmpData[2006+2*i+1];
    }
    data.s1.tail=tmpData[3000];
    data.s1.head=tmpData[3002];
    data.s1.len=tmpData[3005];
    for(int i=0; i<76; i++)
    {
        data.s1.body[i].x=tmpData[3006+2*i];
        data.s1.body[i].y=tmpData[3006+2*i+1];
    }
    return 0;
}

int DLL_EXPORT player_1(int *tmpData)
{
    Init_1(tmpData);
    //算法部分，由于改写了那个同学的初始化代码，此处算法在player_2函数中一样即可，直接复制粘贴
}

int DLL_EXPORT player_2(int *tmpData)
{
    Init_2(tmpData);
    //算法部分，由于改写了那个同学的初始化代码，此处算法在player_1函数中一样即可，直接复制粘贴
}
