
#ifndef USELEI_H_INCLUDED
#define USELEI_H_INCLUDED

#define LENBODY 100
#define LENM 50

class Point
{
public:
    int x,y;
    Point()
    {
        x=0;
        y=0;
    }
    Point(int x_,int y_)
    {
        x=x_;
        y=y_;
    }
    Point(const Point &p_)
    {
        x=p_.x;
        y=p_.y;
    }
    void operator=(Point p_)
    {
        x=p_.x;
        y=p_.y;
    }
    bool operator==(Point p_)
    {
        return ((bool)(x==p_.x && y==p_.y));
    }
    Point operator+(Point p_)
    {
        return Point(x+p_.x,y+p_.y);
    }
    Point operator+=(Point p_)
    {
        return Point(x+p_.x,y+p_.y);
    }
private:
protected:
};

class Snake
{
public:
    int head,len,tail;
    Point body[LENBODY];
    Snake()
    {
        head=0;
        len=0;
        tail=0;
        for(int i=0; i<LENBODY; i++)
        {
            body[i]=Point();
        }
    }
    Snake(const Snake &s_)
    {
        head=s_.head;
        len=s_.len;
        tail=s_.tail;
        for(int i=0; i<=len; i++)
        {
            body[i]=s_.body[i];
        }
    }
    void operator=(Snake s_)
    {
        head=s_.head;
        len=s_.len;
        tail=s_.tail;
        for(int i=0; i<=len; i++)
        {
            body[i]=s_.body[i];
        }
    }
private:
protected:
};

class Data
{
public:
    int mSize,m[LENM][LENM];
    Point food;
    Snake s1,s2;
    Data()
    {
        mSize=0;
        for(int i=0; i<LENM; i++)
        {
            for(int j=0; j<LENM; j++)
            {
                m[i][j]=0;
            }
        }
        food=Point();
        s1=Snake();
        s2=Snake();
    }
    Data(const Data &d_)
    {
        mSize=d_.mSize;
        for(int i=0; i<=mSize; i++)
        {
            for(int j=0; j<=mSize; j++)
            {
                m[i][j]=d_.m[i][j];
            }
        }
        food=d_.food;
        s1=d_.s1;
        s2=d_.s2;
    }
    void operator=(Data d_)
    {
        mSize=d_.mSize;
        for(int i=0; i<=mSize; i++)
        {
            for(int j=0; j<=mSize; j++)
            {
                m[i][j]=d_.m[i][j];
            }
        }
        food=d_.food;
        s1=d_.s1;
        s2=d_.s2;
    }
private:
protected:
};

#endif
