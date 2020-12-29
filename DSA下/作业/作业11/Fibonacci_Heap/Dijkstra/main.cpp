#include "Fibonacci_Heap.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <queue>
#include <ctime>
#include <time.h>
#include <stdlib.h>
#include "binary_heap.h"

using namespace std;


struct State{
	int d, x;
	State(int d = 0, int x = 0) : d(d), x(x) {}
	const bool operator < (const State& A) const {
		if(d != A.d)return d < A.d;
        else return x < A.x;
	}
    const bool operator <= (const State& A) const {
        if(d != A.d)return d <= A.d;
        else return x < A.x;
    }
	const bool operator == (const State&A){
		return A.d == this->d;
	}
	const bool operator > (const State& A) const {
		if(d != A.d)return d > A.d;
        else return x > A.x;
	}
    const bool operator >= (const State& A) const {
        if(d != A.d)return d >= A.d;
        else return x > A.x;
    }
};

ostream & operator << (ostream &x, const State& A) {
	x << A.d << ' ' << A.x << endl;
	return x;
}

struct cmp {
	bool operator()(State& A, State& B) { return  A.d > B.d; }
};


const int MAXN = 30000000;    // num of edge
const int N = 20000;		// num of node


struct Edge {
	int v, val, next;
	Edge(int v = 0, int val = 0, int next = 0) :v(v), val(val), next(next) {}
}e[MAXN];


int id, head[N], d[N], n, m, par[N];
bool vis[N];


inline void addedge(int u, int v, int val) {
	e[++id] = Edge(v, val, head[u]);
	head[u] = id;
}

void Dijkstra_Binary_heap(int s) {
	MinHeap<State> *Q=new MinHeap<State>(MAXN);
    memset(d, N, sizeof(d));
	d[s] = 0;
	memset(vis, false, sizeof(vis));
    for(int i = 0; i < n; ++i) {
       	Q->insert(State(d[i], i));
    }
	while(!Q->empty()) {
        State tmp = Q->find_min();
		Q->remove(tmp);
		int x = tmp.x;
        if(vis[x]){
            continue;
        }
		vis[x] = true;
        for(int i = head[x]; i; i = e[i].next){
			if (d[e[i].v] > d[x] + e[i].val) {
				par[e[i].v] = x;
				d[e[i].v] = d[x] + e[i].val;
				Q->insert(State(d[e[i].v], e[i].v));
			}
        }
	}
}

void Dijkstra_Fibonacci_heap(int s, int n) {
	int sizess = 1000;
	Fibonacci_Heap<State> Q;
	memset(d, N, sizeof(d));
	d[s] = 0;
    for(int i = 0; i < n; ++i) {
        Q.Push(i, State(d[i], i));
    }
	while(!Q.Empty()) {
		State tmp = Q.Top();
		Q.Pop();
		int x = tmp.x;
        for(int i = head[x]; i; i = e[i].next){
			if (d[e[i].v] > d[x] + e[i].val) {
				par[e[i].v] = x;
				Q.Decrease_Key(e[i].v, State(d[x] + e[i].val, e[i].v));
				d[e[i].v] = d[x] + e[i].val;
			}
        }
	}
}

void init() {
	id = 0;
	memset(head, 0, sizeof(head));
	memset(par, -1, sizeof(par));
}

void print_path(ofstream& ofile, int x) {
	if(par[x] == -1) {
		ofile << x;
		return;
	}
	print_path(ofile, par[x]);
	ofile << "->" << x;
}

int main() {
    ifstream ifile("./graph.txt");
	ofstream ofile("./ans.out");

	//Read data
	init();
    int from,to;
    double weight;
    ifile>>n>>m;
    while(ifile>>from>>to>>weight){
        addedge(from, to, weight);
    }

    int num_src=1;

    //Binary Heap
	ofile << "Binary Heap" << endl;
	ofile << "-----------------" << endl;
    clock_t t_binary_s=clock();
    for(int src=0;src<num_src;src++){
        Dijkstra_Binary_heap(src);
    }
    clock_t t_binary_e=clock();
	for(int i = 1; i < n; ++i) {
		ofile << "[" << d[i] << "] ";
		print_path(ofile, i);
		ofile << endl;
	}
    double t_binary=(t_binary_e-t_binary_s)/(double)CLOCKS_PER_SEC;
	ofile << "-----------------" << endl;

    //Fibonacci Heap
	memset(par, -1, sizeof(par));
	ofile << "Fibonacci Heap" << endl;
	ofile << "-----------------" << endl;
    clock_t t_fib_s=clock();
    for(int src=0;src<num_src;src++){
        Dijkstra_Fibonacci_heap(src, n);
    }
    clock_t t_fib_e=clock();

	for(int i = 1; i < n; ++i) {
		ofile << "[" << d[i] << "] ";
		print_path(ofile, i);
		ofile << endl;
	}

    double t_fib=(t_fib_e-t_fib_s)/(double)CLOCKS_PER_SEC;
    ofile << "-----------------" << endl;

    ofile <<"Time binary heap: " <<t_binary<<" s"<<endl;
    ofile <<"Time fibonacci heap: " <<t_fib<<" s"<<endl;


	ifile.close();
	ofile.close();
	return 0;
}
