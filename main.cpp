#include "main.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <ctime>
//#define D 2
//#define L 3
//#define R 1
//#define U 0

#define NUM1 1
#define NUM2 3
#define NUMFOOD 2
#define NUMNULL 0

using namespace std;
const int MAXN = 40;
const int MAX_ROUND = 100;
const int LENDIR = 4;
const int inf = 1e4;
const int dx[4] = {-1,0,1,0};
const int dy[4] = {0,1,0,-1};

struct _Point
{
    int x, y;
    _Point () {}
    _Point (int _x,int _y)//赋值3
    {
        x = _x;
        y = _y;
    }
    friend bool operator == (const _Point &a, const _Point &b)
    {
        return a.x == b.x && a.y == b.y;
    }
    friend ostream& operator << (ostream& out, const _Point &p)
    {
        return out << '(' << p.x << ',' << p.y << ')';
    }
};

_Point next (const _Point &p, int k)
{
    return _Point(p.x + dx[k], p.y + dy[k]);
}

struct _Snake
{
    _Point pos[MAX_ROUND];
    int begin, end, len;
    //pos[end+1]...pos[begin]是蛇
    //round = begin + 1
public:
    _Point head () const
    {
        return pos[begin];
    }
    _Point tail () const
    {
        return pos[end + 1];
    }
    void move (int k)
    {
        int round = begin + 1;
        pos[begin + 1] = next(pos[begin], k);
        ++begin;
        ++end;
    }
    void withdraw ()
    {
        int lastRound = begin;
        --begin;
        --end;
    }
    friend ostream& operator << (ostream& out, const _Snake &s)
    {
        for(int i = s.begin; i !=  s.end; --i)
            out << s.pos[i] << ' ';
        return out << endl;
    }
};

int map[MAXN][MAXN]; //0:空 1:己方 3:敌方 2:食物
_Point food;
int mSize;
bool liv1;
bool liv2;
_Snake s1;
_Snake s2;

int Init_1(int *tmpData)
{
    food.x = tmpData[0];
    food.y = tmpData[1];
    mSize = tmpData[4];
    for(int i = 0; i<40; i++)
    {
        for(int j = 0; j<40; j++)
        {
            map[i][j] = tmpData[40+i*40+j];
            if      (map[i][j] == NUM1)
                {liv1 = true;}
            else if (map[i][j] == NUM2)
                {liv2 = true;}
        }
    }
    _Snake tmp;
    tmp.end = tmpData[2000];//len-1
    tmp.begin = tmpData[2002];//0
    tmp.len = tmpData[2005];
    for(int i = 0; i<76; i++)
    {
        tmp.pos[i].x = tmpData[2006+2*i];
        tmp.pos[i].y = tmpData[2006+2*i+1];
    }
    for(int i = 0; i<tmp.len; i++)
    {
        s1.pos[i] = tmp.pos[tmp.len-1-i];
    }
    s1.len = tmp.len;
    s1.end = -1;
    s1.begin = tmp.len-1;

    tmp.end = tmpData[3000];//len-1
    tmp.begin = tmpData[3002];//0
    tmp.len = tmpData[3005];
    for(int i = 0; i<76; i++)
    {
        tmp.pos[i].x = tmpData[3006+2*i];
        tmp.pos[i].y = tmpData[3006+2*i+1];
    }
    for(int i = 0; i<tmp.len; i++)
    {
        s2.pos[i] = tmp.pos[tmp.len-1-i];
    }
    s2.len = tmp.len;
    s2.end = -1;
    s2.begin = tmp.len-1;

    return 0;
}

int Init_2(int *tmpData)
{
    food.x = tmpData[0];
    food.y = tmpData[1];
    mSize = tmpData[4];
    for(int i = 0; i<40; i++)
    {
        for(int j = 0; j<40; j++)
        {
            map[i][j] = tmpData[40+i*40+j];
            if      (map[i][j] == NUM1)
                {liv2 = true;}
            else if (map[i][j] == NUM2)
                {liv1 = true;}
        }
    }
    _Snake tmp;
    tmp.end = tmpData[2000];//len-1
    tmp.begin = tmpData[2002];//0
    tmp.len = tmpData[2005];
    for(int i = 0; i<76; i++)
    {
        tmp.pos[i].x = tmpData[2006+2*i];
        tmp.pos[i].y = tmpData[2006+2*i+1];
    }
    for(int i = 0; i<tmp.len; i++)
    {
        s1.pos[i] = tmp.pos[tmp.len-1-i];
    }
    s2.len = tmp.len;
    s2.end = -1;
    s2.begin = tmp.len-1;

    tmp.end = tmpData[3000];//len-1
    tmp.begin = tmpData[3002];//0
    tmp.len = tmpData[3005];
    for(int i = 0; i<76; i++)
    {
        tmp.pos[i].x = tmpData[3006+2*i];
        tmp.pos[i].y = tmpData[3006+2*i+1];
    }
    for(int i = 0; i<tmp.len; i++)
    {
        s2.pos[i] = tmp.pos[tmp.len-1-i];
    }
    s1.len = tmp.len;
    s1.end = -1;
    s1.begin = tmp.len-1;

    return 0;
}

int DLL_EXPORT player_1(int *tmpData)
{
    Init_1(tmpData);
    //算法部分，由于改写了那个同学的初始化代码，此处算法在player_2函数中一样即可，直接复制粘贴
    return 1;
}

int DLL_EXPORT player_2(int *tmpData)
{
    Init_2(tmpData);
    //算法部分，由于改写了那个同学的初始化代码，此处算法在player_1函数中一样即可，直接复制粘贴
    return 2;
}
