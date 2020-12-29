#pragma once

#include <vector>
#include <cstring>
#include <algorithm>
#include <map>
#include <iostream>
using namespace std;

template <class T>
class Fibonacci_Heap {
private:
	struct Node {
		T key;
		int degree;
		bool mark;
		Node *p, *child, *left, *right;
		Node(T k) : key(k), degree(0), mark(false) {
			p = child = nullptr;
			left = right = this;
		}
	};

	Node *Min;
	int n;
	//map<T, Node*> mp;
    vector<Node*>mp;
	void Del_Tree(Node *root);
	void Consolidate();
	void Link(Node *y, Node *x);
	void Cut(Node *x, Node *y);
	void Cascading_Cut(Node *y);
public:
	Fibonacci_Heap();
	~Fibonacci_Heap();

	void Push(int id,T x);
	bool Empty();
	T Top();
	void Pop();
	void Decrease_Key(int id, T k);
};

template <class T>
Fibonacci_Heap<T>::Fibonacci_Heap() {
    Min = nullptr;
    n = 0;
}

template <class T>
void Fibonacci_Heap<T>::Del_Tree(Node *root) {
    if(root -> child != nullptr) {
        Node *ptr = root -> child;
        do {
            Del_Tree(ptr);
            ptr = ptr -> right;
        } while(ptr != root -> child);
    }
    delete root;
}

template <class T>
Fibonacci_Heap<T>::~Fibonacci_Heap() {
    mp.clear();
    Node *ptr = Min;
    if(ptr == nullptr)return;
    do {
        Del_Tree(ptr);
        ptr = ptr -> right;
    } while(ptr != Min);
}

template <class T>
void Fibonacci_Heap<T>::Push(int id,T x) {
    while(id >= mp.size()){
        mp.push_back(nullptr);
    }
    Node* point = new Node(x);
    mp[id] = point;
    
    if(n == 0){
        Min = point;
    }else{
        Node* tmp = Min->left;
        tmp->right = point;
        Min->left = point;
        point->left = tmp;
        point->right = Min;
        if(Min->key > point->key)
        {
            Min = point;
        }
    }
    n++;
}

template <class T>
bool Fibonacci_Heap<T>::Empty() {
    if(n == 0)return true;
    else return false;
    
}

template <class T>
T Fibonacci_Heap<T>::Top() {
    return Min->key;
    
}

template <class T>
void Fibonacci_Heap<T>::Pop() {
   
    if(n == 0) return;
    n--;
    if(n == 0) {
        delete Min;
        Min = nullptr;
        return;
    }
    Node *tmp = Min -> child;
    vector <Node *> chdlist;  
    if(tmp != nullptr)
        do{
            chdlist.push_back(tmp);
            tmp = tmp -> right;
        }while(tmp != Min -> child);
    for(int i = 0; i < chdlist.size(); i++){
        Node *iterat = chdlist[i];
        Node *Mleft = Min -> left;
        iterat -> p = nullptr;
        Mleft -> right = iterat;    Min -> left = iterat;
        iterat -> left = Mleft;     iterat -> right = Min;
    }
    Node *l = Min -> left;
    Node *r = Min -> right;
    l -> right = r;
    r -> left = l;
    delete Min;
    Min = l;
    Consolidate();
}

template <class T>
void Fibonacci_Heap<T>::Consolidate() {
    //cout<<"consolidate"<<endl;
    vector<Node*>root_list;
    vector<Node*>A;
    Node* cur = Min->right;
    root_list.push_back(Min);
    while(cur != Min)
    {
        while(cur->degree+1 > A.size())
        { A.push_back(nullptr); }
        root_list.push_back(cur);
        cur = cur->right;
    }
    
    for(int i = 0;i < root_list.size();i++)
    {
        Node* x = root_list[i];
        int d = x->degree;
        while(d + 10>A.size()){A.push_back(nullptr);}
        //cout<<d<<endl;
        //cout<<A.size()<<endl;
        while(A[d] != nullptr)
        {   
            Node* y = A[d];
            if(x->key > y->key)
            {
                Node* swap = x;
                x = y;
                y = swap;
            }
            Link(y,x);
            A[d] = nullptr;
            d++;
        }
        while(d+5 > A.size()){ A.push_back(nullptr);}
        A[d] = x;
    }
    Min = nullptr;
    for(int j = 0;j < A.size();j++)
    {
        if(A[j]!=nullptr){
            if(Min == nullptr)
            {
                Min = A[j];
                Min->left = Min;Min->right = Min;
            }
            else
            {
                
                Node* le = Min->left;
                le -> right = A[j];
                Min -> left = A[j];
                A[j]->right = Min;
                A[j]->left = le;
                if(Min->key > A[j]->key)Min = A[j];
            }
            
        }
    }
}

template <class T>
void Fibonacci_Heap<T>::Link(Node *y, Node *x) {
    //remove y from the root list
    Node* l = y->left;
    Node* r = y->right;
    l->right = r;
    r->left = l;

    //make y a child of x, incrementing x.degree
    x->degree++;
    y->p = x;
    y->mark = false;
    if(x->child != nullptr){
        Node* t = x->child->right;
        t->left = y;
        y->left = x->child;
        y->right = t;
        x->child->right = y;
    }else
    {
        x->child = y;
        y->left = y;y->right = y;
    }
    
}

template<class T>
void Fibonacci_Heap<T>::Decrease_Key(int id, T k) {

    if(id>=mp.size()||mp[id] == nullptr){
        cout<<"The target  doesn't exit"<<endl;
        return;
    }
    if(mp[id]->key < k){
        cout<<"The key of target  is higher than you thought"<<endl;return;
    }
    Node* target = mp[id];
    target->key = k;
    Node* fa = target->p;
    if(fa!=nullptr && target->key < fa->key)
    {
        Cut(target,fa);
        Cascading_Cut(fa);
    }
    if(target->key < Min->key)
        Min = target;
}

template <class T>
void Fibonacci_Heap<T>::Cut(Node *x, Node *y) {
    y->degree--;
    

    //remove x from child list of y
    Node* tmp = x;
    if(y->degree == 0){y->child = nullptr;}
    else{
        if(y->child == x)
            y->child = x->right; 
        Node* left = x->left;
        Node* right = x->right;
        right->left = left;
        left->right = right;
        
    }

    //add x to the root list
    x->p = nullptr;
    x->mark = false;
    Node* temp = Min->left;
        temp->right = x;
        Min->left = x;
        x->left = temp;
        x->right = Min;
}

template <class T>
void Fibonacci_Heap<T>::Cascading_Cut(Node *y) {
    Node* z = y->p;
    if(z!=nullptr){
        if(y->mark == false)
        {
            y->mark = true;
        }else
        {
            Cut(y,z);
            Cascading_Cut(z);
        }
        
    }
}

