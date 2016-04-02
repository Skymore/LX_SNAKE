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
#include "jsoncpp/json.h"
using namespace std;

typedef unsigned long long u64;
const int inf = 1e4;
const int MAXN = 20;
const int dx[4] = {-1,0,1,0};
const int dy[4] = {0,1,0,-1};
const clock_t t0 = clock();
int n, m;

const int MAX_NODE = 1e6;
const int MAX_ROUND = 200;
const bool USE_DATA = false;
const bool READ_DATA_FROM_RESPONSE = false;
const bool NO_DEBUG = false;

Json::Value ret;
Json::FastWriter writer;
void JsonOutput (int dire)
{
	ret["response"]["direction"] = dire;
	cout << writer.write(ret) << endl;
}
void JsonDebug (const string &s)
{
	ret["debug"] = s;
}
void JsonData (const string &s)
{
	ret["data"] = s;
	if(READ_DATA_FROM_RESPONSE)
		ret["response"]["data"] = s;
}

void printByte (const string &s)
{
	const size_t l = s.size();
	for(int i=0; i<l; ++i)
		fprintf(stderr, "%02X ", (unsigned char)s[i]);
	cerr << endl;
}
/*
u64 zobrist[2][2][MAXN][MAXN];	//[id][isHead][x][y]
inline u64 rand64 ()
{
	return ((u64)rand() << 48) ^ ((u64)rand() << 24) ^ (u64)rand();
}
void makeZobrist ()
{
	for(int id=0; id<=1; ++id)
		for(int head=0; head<=1; ++head)
			for(int i=1; i<=n; ++i)
				for(int j=1; j<=m; ++j)
					zobrist[id][head][i][j] = rand64();
}
*/

bool keepTail (int round)  //本回合是否生长
{
	if (round <= 25)		return true;
	if ((round-25)%3==0)	return true;
	return false;
}

//okDire用4个二进制位存储可以走的方向，z是可行的方向数，randDire返回随机一个方向 
int randDire (int okDire)  
{
	int d[4], z = 0;
	for(int i=0; i<4; ++i, okDire >>= 1)
		if(okDire & 1)
			d[z++] = i;
	if(z == 0)	return -1;	//无处可逃
	return d[rand() % z];
}
//???
inline int bitcnt (int x)
{
	int cnt = 0;
	for(; x; x >>= 1)
		if(x & 1)
			++cnt;
	return cnt;
}

struct Point
{
	int x, y;
	Point () {}
	Point (int _x,int _y)//赋值3
	{
		x = _x;
		y = _y;
	}
	friend bool operator == (const Point &a, const Point &b)
	{
		return a.x == b.x && a.y == b.y;
	}
	friend ostream& operator << (ostream& out, const Point &p)
	{
		return out << '(' << p.x << ',' << p.y << ')';
	}
};

Point next (const Point &p, int k)
{
	return Point(p.x + dx[k], p.y + dy[k]);
}

struct Snake
{
	Point pos[MAX_ROUND];
	int begin, end;
	//round = begin + 1
public:
	void born (Point p0)
	{
		pos[0] = p0;
		begin = 0, end = -1; 
		//pos[end+1]...pos[begin]是蛇
	}
	Point head () const
	{
		return pos[begin];
	}
	Point tail () const
	{
		return pos[end + 1];
	}
	void move (int k)
	{
		int round = begin + 1;
		pos[begin + 1] = next(pos[begin], k);
		++begin;
		if(!keepTail(round))
			++end;
	}
	void withdraw ()
	{
		int lastRound = begin;
		--begin;
		if(!keepTail(lastRound))
			--end;
	}
	void makeLifeMap (int life[MAXN][MAXN]) const
	{
		int round = begin + 1;
		for(int t = 1, p = end + 1; p != begin + 1; ++t)
			if(!keepTail(round + t-1))
				life[pos[p].x][pos[p].y] = t, ++p;
	}
	friend ostream& operator << (ostream& out, const Snake &s)
	{
		for(int i = s.begin; i != s.end; --i)
			out << s.pos[i] << ' ';
		return out << endl;
	}
};

struct PointWithLife
{
	Point p;
	int life;
	PointWithLife () {}
	PointWithLife (Point _p, int _life): p(_p), life(_life) {}
	bool operator < (const PointWithLife &a) const
	{
		return life > a.life;
	}
};

class QueuePlus
{
	priority_queue<PointWithLife> Q1;
	PointWithLife Q0[MAXN * MAXN];
	int head, tail;
public:
	QueuePlus ()
	{
		head = tail = 0;
	}
	void push (const PointWithLife &pl)
	{
		if(pl.life == 0)
			Q0[tail++] = pl;
		else
			Q1.push(pl);
	}
	void pop ()
	{
		if(head != tail)
			++head;
		else
			Q1.pop();
	}
	PointWithLife top () const
	{
		if(head != tail)
			return Q0[head];
		else
			return Q1.top();
	}
};

int maxRemain (Point p0, const int life[MAXN][MAXN], int up, bool fill[MAXN][MAXN]) //ans < up
{
	bool visit[MAXN][MAXN] = {0};
	bool inside[MAXN][MAXN] = {0};
	int near[MAXN][MAXN] = {0};
	int countNear[5] = {0};
	int count01[2] = {0};
	//priority_queue<PointWithLife> qEdge;
	QueuePlus qEdge;
	qEdge.push( PointWithLife(p0, life[p0.x][p0.y]));
	visit[p0.x][p0.y] = true;
	int nowLife = 0, area = 0;
	while(true)
	{
		Point p = qEdge.top().p;
		qEdge.pop();
		if(nowLife == 0)
			fill[p.x][p.y] = true;
		inside[p.x][p.y] = true;
		++countNear[near[p.x][p.y]];
		++count01[(p.x + p.y) & 1];
		++area;
		if(area >= up)
			return inf;
		//cerr << p << endl;
		for(int k=0; k<4; ++k)
		{
			const Point pp = next(p, k);
			int& n = near[pp.x][pp.y];
			if(inside[pp.x][pp.y])
				--countNear[n], ++countNear[n+1];
			++n;
			if(!visit[pp.x][pp.y])
				visit[pp.x][pp.y] = true,
				qEdge.push(PointWithLife(pp, life[pp.x][pp.y]));
		}
		int minLife = qEdge.top().life;
		if(minLife > nowLife)
		{
			//Judge
			int realArea;
			if(countNear[1] - (near[p0.x][p0.y] == 1) >= 2)
				realArea = area - (countNear[1] - (near[p0.x][p0.y] == 1) - 1);
			else
				realArea = area;
			realArea = min(realArea, min(count01[0], count01[1])*2 + 1);
			
			/*
			for(int i=1; i<=4; ++i)
				cerr << countNear[i] << ' ';
			cerr << endl;
			cerr << "minLife = " << minLife << "\trealArea + 1 = " << realArea+1 << endl;
			*/
			
			if(realArea + 1 < minLife)
				return realArea;
			nowLife = minLife;
		}
	}
}

bool preJudge = true;
struct Situation
{
	Snake s[2];
	int round;
	//u64 hash;
	char map[MAXN][MAXN];	//0:空 1:己方 2:敌方 3:障碍
	
public:
	int getMaxRemain (int id, const int life[MAXN][MAXN], int up) const
	{
		int ans = 0;
		int ok = getOkDire(id);
		bool fill[MAXN][MAXN] = {0};
		for(int k=0; k<4; ++k)
			if(ok & (1 << k))
			{
				Point pp = next(s[id].head(), k);
				if(fill[pp.x][pp.y])
					continue;
				ans = max(ans, maxRemain(pp, life, up, fill));
			}
		return ans;
	}
	bool makeDomin (const int life[MAXN][MAXN], int dist[2][MAXN][MAXN]) const
	//return whether share border
	{
		bool shareBorder = false;
		Point p0 = s[0].head();
		Point p1 = s[1].head();
		
		Point Q[MAXN * MAXN];
		int head = 0, tail = 2;
		Q[0] = p0, Q[1] = p1;
		
		for(int id=0; id<=1; ++id)
			for(int i=0; i<=n+1; ++i)
				for(int j=0; j<=m+1; ++j)
					dist[id][i][j] = inf;
		dist[0][p0.x][p0.y] = 0;
		dist[1][p1.x][p1.y] = 0;
		
		while(head != tail)
		{
			const Point &p = Q[head++];
			const bool id = (dist[0][p.x][p.y] == inf);
			const int d = dist[id][p.x][p.y];
			for(int k=0; k<4; ++k)
			{
				const Point pp = next(p, k);
				int &dd = dist[id][pp.x][pp.y];
				if(dist[!id][pp.x][pp.y] != inf)
					shareBorder = true;
				else if(life[pp.x][pp.y] <= d + 1 && dd == inf && dist[!id][pp.x][pp.y] == inf)
				{
					dd = d + 1;
					Q[tail++] = pp;
				}
			}
		}
		return shareBorder;
	}
	
	void makeLifeMap (int life[MAXN][MAXN]) const
	{
		for(int i=0; i<=n+1; ++i)
			for(int j=0; j<=m+1; ++j)
				life[i][j] = map[i][j]? inf: 0;
		s[0].makeLifeMap(life);
		s[1].makeLifeMap(life);
	}
	
	int evaluate () const
	{
		int life[MAXN][MAXN];
		int dist[2][MAXN][MAXN];
		makeLifeMap(life);
		bool shareBorder = makeDomin(life, dist);
		
		if(!shareBorder && preJudge)
		{
			int minRemain0 = 0, minRemain1 = 0;
			int Sdomin0 = 0, Sdomin1 = 0;
			for(int i=1; i<=n; ++i)
				for(int j=1; j<=m; ++j)
					if(dist[0][i][j] != inf)
						++Sdomin0,
						minRemain0 = max(minRemain0, dist[0][i][j]);
					else if(dist[1][i][j] != inf)
						++Sdomin1,
						minRemain1 = max(minRemain1, dist[1][i][j]);
			minRemain0 = max(minRemain0, Sdomin0 *3/4);
			minRemain1 = max(minRemain1, Sdomin1 *3/4);
			int maxRemain0 = getMaxRemain(0, life, minRemain1);
			int maxRemain1 = getMaxRemain(1, life, minRemain0);
			if(maxRemain0 < minRemain1)
				return -inf;
			else if(maxRemain1 < minRemain0)
				return inf;
		}
		
		int s = 0;
		for(int i=1; i<=n; ++i)
			for(int j=1; j<=m; ++j)
				if(dist[0][i][j] != inf)
				{
					s += 4;
					for(int k=0; k<4; ++k)
						if(dist[0][i+dx[k]][j+dy[k]] != inf)
							++s;
				}
				else if(dist[1][i][j] != inf)
				{
					s -= 4;
					for(int k=0; k<4; ++k)
						if(dist[1][i+dx[k]][j+dy[k]] != inf)
							--s;
				}
		for(int i=1; i<=n; ++i)
			for(int j=1; j<=m; ++j)
				if(map[i][j] == 1)
				{
					for(int k=0; k<4; ++k)
						if(map[i+dx[k]][j+dy[k]])
							s += 5;
				}
				else if(map[i][j] == 2)
				{
					for(int k=0; k<4; ++k)
						if(map[i+dx[k]][j+dy[k]])
							s -= 5;
				}
		return s;
	}
	
	int end () const
	//1:我方败 2:敌方败 3:同时败
	{
		return 1 * (getOkDire(0) == 0) + 2 * (getOkDire(1) == 0);
	}
	
	void go (int dire0, int dire1)
	{
		if(!keepTail(round++))
		{
			Point t0 = s[0].tail();
			Point t1 = s[1].tail();
			map[t0.x][t0.y] = 0;
			map[t1.x][t1.y] = 0;
			//hash ^= zobrist[0][0][t0.x][t0.y];
			//hash ^= zobrist[1][0][t1.x][t1.y];
		}
		Point h0 = s[0].head();
		Point h1 = s[1].head();
		Point p0 = next(h0, dire0);
		Point p1 = next(h1, dire1);
		map[p0.x][p0.y] = 1;
		map[p1.x][p1.y] = 2;
		/*
		hash ^= zobrist[0][1][h0.x][h0.y];
		hash ^= zobrist[1][1][h1.x][h1.y];
		hash ^= zobrist[0][0][h0.x][h0.y];
		hash ^= zobrist[1][0][h1.x][h1.y];
		hash ^= zobrist[0][1][p0.x][p0.y];
		hash ^= zobrist[1][1][p1.x][p1.y];
		*/
		
		s[0].move(dire0);
		s[1].move(dire1);
	}
	
	void withdraw ()
	{
		Point p0 = s[0].head();
		Point p1 = s[1].head();
		map[p0.x][p0.y] = 0;
		map[p1.x][p1.y] = 0;
		s[0].withdraw();
		s[1].withdraw();
		//Point h0 = s[0].head();
		//Point h1 = s[1].head();
		/*
		hash ^= zobrist[0][1][h0.x][h0.y];
		hash ^= zobrist[1][1][h1.x][h1.y];
		hash ^= zobrist[0][0][h0.x][h0.y];
		hash ^= zobrist[1][0][h1.x][h1.y];
		hash ^= zobrist[0][1][p0.x][p0.y];
		hash ^= zobrist[1][1][p1.x][p1.y];
		 */
		
		if(!keepTail(--round))
		{
			Point t0 = s[0].tail();
			Point t1 = s[1].tail();
			map[t0.x][t0.y] = 1;
			map[t1.x][t1.y] = 2;
			//hash ^= zobrist[0][0][t0.x][t0.y];
			//hash ^= zobrist[1][0][t1.x][t1.y];
		}
	}
	
	int getOkDire(int id) const
	{
		Point p = s[id].head();
		int ans = 0;
		for(int k=0; k<4; ++k)
			if(valid(next(p, k)))
				ans |= 1 << k;
		return ans;
	}
	
	bool valid (Point p) const
	{
		//判断下回合消失的蛇尾
		int id = map[p.x][p.y] - 1;
		if(!keepTail(round) && (id == 0 or id == 1) && p == s[id].tail())
			return true;
		//正常情况
		return !map[p.x][p.y];
	}
	
	void print () const
	{
		const char cc[] = " 12*";
		for(int j=0; j<=m+1; ++j)
		{
			for(int i=0; i<=n+1; ++i)
				cerr << cc[map[i][j]];
			cerr << endl;
		}
	}
	/*
	void go (char* s)
	{
		int k0, k1;
		while(true)
		{
			while(*s && !isnumber(*s))
				++s;
			if(*s == 0)
				return;
			k0 = *s++ - '0';
			while(*s && !isnumber(*s))
				++s;
			if(*s == 0)
				return;
			k1 = *s++ - '0';
			go(k0, k1);
		}
	}
	 */
};

struct Node
{
	char status;		//-1:middle 0:normal 1:lose 2:win 3:equal
	char bestChoice;
	int score;
	int evaluation;		//valid if normal
	Node *next[4];
	void makeNormal (const Situation &S)
	{
		status = S.end();
		if(status > 0)
		{
			const int s0[4] = {0, -inf, inf, -inf/2};
			score = s0[status];
			bestChoice = randDire(S.getOkDire(0));
		}
		else
		{
			score = evaluation = S.evaluate();
			bestChoice = -1;
		}
		memset(next, 0, sizeof(next));
	}
	void makeMiddle ()
	{
		status = -1;
		score = evaluation = 0;
		bestChoice = -1;
		memset(next, 0, sizeof(next));
	}
	Node* getNext (int k, const Situation &S)
	{
		Node* &p = next[k];
		if(p == NULL)
		{
			p = new Node;
			if(status == -1)
				p->makeNormal(S);
			else
				p->makeMiddle();
		}
		return p;
	}
	void printChoiceLine () const
	{
		for(const Node *p = this; p != NULL;)
		{
			int k = p->bestChoice;
			if(k == -1)	break;
			if(p->status == -1)
				fprintf(stderr, ",%d) ", k);
			else
				fprintf(stderr, "(%d", k);
			p = p->next[k];
		}
		fprintf(stderr, "\n");
	}
	void print () const
	{
		fprintf(stderr, "Node Info:\nstatus = %d  score = %d  evaluation = %d\n", status, (int)score, evaluation);
		fprintf(stderr, "son = ");
		for(int k=0; k<4; ++k)
			if(next[k] != NULL)
				fprintf(stderr, "%d ", k);
		fprintf(stderr, "\n\n");
	}
	void dfs (int d = inf)
	{
		if(d == 0)
		{
			fprintf(stderr, "...\n");
			return;
		}
		for(int k=0; k<4; ++k)
		{
			if(next[k] == NULL)
				continue;
			for(int i=0; i<d; ++i)
				fprintf(stderr, "  ");
			if(status == -1)
				fprintf(stderr, "[%d]", k);
			else
				fprintf(stderr, "(%d)", k);
			fprintf(stderr, "  %d\n", next[k]->score);
			next[k]->dfs(d - 1);
		}
	}
	int size () const
	{
		int s = 1;
		for(int k=0; k<4; ++k)
			if(next[k] != NULL)
				s += next[k]->size();
		return s;
	}
	bool leaf () const
	{
		for(int k=0; k<4; ++k)
			if(next[k] != NULL)
				return false;
		return true;
	}
	void makeNextList (char *c, const Situation &S) const
	{
		int z = 0;
		int ok = S.getOkDire(status == -1);
		if(bestChoice == -1)
		{
			for(int k=0; k<4; ++k)
				if(ok & (1 << k))
					c[z++] = k;
		}
		else
		{
			c[z++] = bestChoice;
			for(int k=0; k<4; ++k)
				if(k != bestChoice && ok & (1 << k))
					c[z++] = k;
		}
		c[z] = -1;
	}
	void* operator new (size_t size)
	{
		static Node pool[MAX_NODE], *p = pool;
		if(p - pool == MAX_NODE)
			throw bad_alloc();
		return p++;
	}
};

void NodeToString (Node *p, string &s, int d)
{
	if(d == 0 || p == NULL)
	{
		s.push_back('*');
		return;
	}
	static char str[15];
	sprintf(str, "%c%c%d ", p->status+'1', p->bestChoice+'1', p->evaluation);
	s += str;
	if(p->leaf())
		s.push_back('#');
	else
		for(int k=0; k<4; ++k)
			NodeToString(p->next[k], s, d - 1);
}
Node* StringToNode (const char* &s)
{
	if(*s == '*' || *s == 0)
	{
		++s;
		return NULL;
	}
	Node *p = new Node;
	sscanf(s, "%c%c%d", &p->status, &p->bestChoice, &p->evaluation);
	p->status -= '1';
	p->bestChoice -= '1';
	p->score = p->evaluation;
	while(*s != ' ')
		++s;
	++s;
	if(*s == '#')
		for(int k=0; k<4; ++k)
			p->next[k] = NULL;
	else
		for(int k=0; k<4; ++k)
			p->next[k] = StringToNode(s);
	return p;
}
void ignoreToNext (const char* &s, int k)
{
	if(*s == '*')
	{
		++s;
		return;
	}
	while(*s != ' ')
		++s;
	++s;
	for(int i=0; i<k; ++i)
		ignoreToNext(s, 4);
}

int minmax (Situation &S, Node *node, const int r0, int depth, int alpha, int beta, int &cnt)
//search situation whose evalutaion is in (alpha, beta)
{
	++cnt;
	if(clock() > 950000)
		throw "out_of_time";
	
	if(node->status > 0)	//if end
	{
		const int s0[4] = {0, -inf, inf, -inf/2};
		return s0[node->status];
	}
	if(depth <= 0 || node->evaluation == inf || node->evaluation == -inf)
		return node->evaluation;
	
	int best = -inf;
	char c[5];
	node->makeNextList(c, S);
	for(char *cc = c; *cc != -1; ++cc)
	{
		const int k = *cc;
		int &val = node->score;
		if(r0 == -1)
			val = -minmax(S, node->getNext(k, S), k, depth, -beta, -alpha, cnt);
		else
		{
			S.go(r0, k);
			val = -minmax(S, node->getNext(k, S), -1, depth - 1, -beta, -alpha, cnt);
			S.withdraw();
		}
		if(val >= beta)
			return val;
		if(val > best)
			best = val, node->bestChoice = k;
		alpha = max(alpha, val);
	}
	return alpha;
}

void debug (Situation stu)
{
	int life[MAXN][MAXN];
	stu.makeLifeMap(life);
	int dist[2][MAXN][MAXN];
	stu.makeDomin(life, dist);
	
	cerr << "LifeMap:" << endl;
	for(int j=0; j<=m+1; ++j)
	{
		for(int i=0; i<=n+1; ++i)
			if(life[i][j] == inf)
				cerr << "*\t";
			else
				cerr << life[i][j] << '\t';
		cerr << endl;
	}
	cerr << endl;
	
	cerr << "DominNew:" << endl;
	for(int j=0; j<=m+1; ++j)
	{
		for(int i=0; i<=n+1; ++i)
			if(dist[0][i][j] != inf)
				fprintf(stderr, "[%d]\t", dist[0][i][j]);
			else if(dist[1][i][j] != inf)
				fprintf(stderr, "(%d)\t", dist[1][i][j]);
			else
				fprintf(stderr, "\t");
		cerr << endl;
	}
	cerr << endl;
}

void deliver (Node *root, int depth)
{
	string s;
	s.push_back(depth + 'A');
	NodeToString(root, s, depth * 2 - 1);
	JsonData(s);
}

stringstream ss;
int AlgoDriver (Situation S, Node* root)
{
	const int ok = S.getOkDire(0);
	if(!USE_DATA && bitcnt(ok) == 1)
		return randDire(ok);
	
	int cnt0 = 0;
	int choice0 = randDire(ok);
	int deliverDepth = 1;
	if(root == NULL)
		(root = new Node)->makeNormal(S);
	for(int d = 1; true; ++d)
	{
		try
		{
			int cnt = 0;
			int score = root->score = minmax(S, root, -1, d, -inf - 1, inf + 1, cnt);
			int choice = root->bestChoice;
			
			if(!NO_DEBUG)
			{
				fprintf(stderr, "depth = %d\t\tcnt = %d\tscore = %d\tchoice = %d\n", d, cnt, score, choice);
				//root->dfs();
				//root->printChoiceLine();
				char str[100];
				sprintf(str, "d=%d cnt=%d s=%d c=%d || ", d, cnt, score, choice);
				ss << str;
			}
			
			if(cnt < 10000)
				deliverDepth = d;
			if(d == 1 && (score == inf || score == -inf))
			{
				preJudge = false;
				(root = new Node)->makeNormal(S);
				continue;
			}
			if(score == -inf)
				break;
			choice0 = choice;
			if(score == inf || cnt == cnt0)
				break;
			cnt0 = cnt;
		}
		catch (char const*)
		{
			break;
		}
	}
	if(USE_DATA)
		deliver(root->next[choice0], deliverDepth);
	return choice0;
}

void readFromJSON (Situation &S, Node* &root)
{
	memset(S.map, 0, sizeof(S.map));
	string str, temp;
	while (getline(cin,temp))
		str += temp;
	
	Json::Reader reader;
	Json::Value input;
	reader.parse(str,input);
	
	n = input["requests"][(Json::Value::UInt) 0]["height"].asInt();  //棋盘高度
	m = input["requests"][(Json::Value::UInt) 0]["width"].asInt();   //棋盘宽度
	//makeZobrist();
	
	int x = input["requests"][(Json::Value::UInt) 0]["x"].asInt();  //读蛇初始化的信息
	S.s[x != 1].born(Point(1,1));
	S.s[x == 1].born(Point(n,m));
	S.map[1][1] = 1 + (x != 1);
	S.map[n][m] = 1 + (x == 1);
	//S.hash = zobrist[x != 1][1][1][1] ^ zobrist[x == 1][1][n][m];
	
	//处理地图中的障碍物
	for(int i=0; i<=n+1; ++i)
		S.map[i][0] = S.map[i][m+1] = 3;
	for(int j=0; j<=m+1; ++j)
		S.map[0][j] = S.map[n+1][j] = 3;
	
	int obsCount = input["requests"][(Json::Value::UInt) 0]["obstacle"].size();
	
	for (int i=0; i<obsCount ;i++)
	{
		int ox = input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["x"].asInt();
		int oy = input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["y"].asInt();
		S.map[ox][oy] = 3;
	}
	
	//根据历史信息恢复现场
	int total = input["responses"].size();
	S.round = 1;
	for (int i=0; i<total; i++)
	{
		int dire0 = input["responses"][i]["direction"].asInt();
		int dire1 = input["requests"][i+1]["direction"].asInt();
		S.go(dire0, dire1);
		
		//cerr << round << endl;
		//print();
	}
	
	//恢复搜索历史树
	if(USE_DATA)
	{
		//string data = (total == 0? string(): input["data"].asString());
		string data;
		if(total)
			data = READ_DATA_FROM_RESPONSE? input["responses"][total-1]["data"].asString(): input["data"].asString();
		if(!data.empty())
		{
			int k1 = input["requests"][total]["direction"].asInt();
			const char* ps = data.c_str() + 1;
			ignoreToNext(ps, k1);
			root = StringToNode(ps);
			cerr << "Successfully read from data." << endl;
		}
	}
}

void jerrMaxRemain (const Situation &S)
{
	int life[MAXN][MAXN];
	S.makeLifeMap(life);
	int maxRemain0 = S.getMaxRemain(0, life, inf);
	int maxRemain1 = S.getMaxRemain(1, life, inf);
	ss << "maxRemain0 = " << maxRemain0 << "\nmaxRemain1 = " << maxRemain1 << endl;
}

int main()
{
//#define LOCAL
#ifdef LOCAL
	ifstream fin("/Users/wangrunji/Codes/Snake2/input.txt");
	cin.rdbuf(fin.rdbuf());
	freopen("/Users/wangrunji/Codes/Snake2/output.txt", "w", stdout);
#endif
	srand((unsigned)time(0));
	
	Situation S;
	Node *root = NULL;
	readFromJSON(S, root);
	
#ifdef LOCAL
	S.print();
	debug(S);
#endif
	//AlgoDriver
	int dire = AlgoDriver(S, root);
	ss << "time = " << clock() - t0 << endl;
	if(!NO_DEBUG)
		JsonDebug(ss.str());
	JsonOutput(dire);

	return 0;
}