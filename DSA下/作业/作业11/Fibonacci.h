/*
 * @Description: 算法导论第19章斐波那契堆实现
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-11-25 21:54:13
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-11-27 14:16:14
 */
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
    struct Node
    {
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
    // vector<Node*> mp;
    void Del_Tree(Node *root);
	void Consolidate();
	void Link(Node *y, Node *x);
	// void Cut(Node *x, Node *y);
	// void Cascading_Cut(Node *y);
public:
    Fibonacci_Heap();
	~Fibonacci_Heap();

	bool Empty();
	T Top();
    
    void Insert(T x);
	T ExtractMin();
	// void Decrease_Key(T k);
    void HeapUnion(Fibonacci_Heap <T> &A, Fibonacci_Heap <T> &B);
};


template <class T>
Fibonacci_Heap<T>::Fibonacci_Heap() {
    this->Min = nullptr;
    this->n = 0;
}


template <class T>
Fibonacci_Heap<T>::~Fibonacci_Heap() {
    // this->mp.clear();
    Node *ptr = this->Min;
    if (ptr == nullptr)return;
    do {
        Del_Tree(ptr);
        ptr = ptr -> right;
    } while(ptr != this->Min);
}


template <class T>
void Fibonacci_Heap<T>::Del_Tree(Node *root) {
    if(root->child != nullptr) {
        Node *ptr = root->child;
        do {
            this->Del_Tree(ptr);
            ptr = ptr->right;
        } while(ptr != root->child);
    }
    delete root;
}


template <class T>
bool Fibonacci_Heap<T>::Empty() {
    if (this->n == 0)
        return true;
    return false;
}


template <class T>
T Fibonacci_Heap<T>::Top() {
    return this->Min->key; 
}


template <class T>
void Fibonacci_Heap<T>::Link(Node *y, Node *x) {
    // 把y从根链表中移除
    Node* l = y->left;
    Node* r = y->right;
    l->right = r;
    r->left = l;
    // 把y作为x的子结点，增加x.degree
    x->degree ++;
    y->p = x;
    y->mark = false;
    if (x->child != nullptr) {
        Node* t = x->child->right;
        t->left = y;
        y->right = t;
        y->left = x->child;
        x->child->right = y;
    }
    else {
        x->child = y;
        y->left = y;
        y->right = y;
    }
}


template <class T>
void Fibonacci_Heap<T>::Consolidate() {
    vector <Node*> root_list;
    vector <Node*> A;
    Node* cur = this->Min->right;
    root_list.push_back(this->Min);
    while (cur != this->Min) {
        while (cur->degree + 1 > A.size()) {
            A.push_back(nullptr);
        }
        root_list.push_back(cur);
        cur = cur->right;
    }
    // 
    for (int i = 0; i < root_list.size(); i ++) {
        Node* x = root_list[i];
        int d = x->degree;
        while (d + 10 > A.size()) {
            A.push_back(nullptr);
        }
        while (A[d] != nullptr) {
            Node* y = A[d];
            if (x->key > y->key) {
                Node* tmp = x;
                x = y;
                y = tmp;
            }
            this->Link(y, x);
            A[d] = nullptr;
            d ++;
        }
        while (d + 5 > A.size()) {
            A.push_back(nullptr);
        }
        A[d] = x;
    }
    this->Min = nullptr;
    for (int i = 0; i < A.size(); i ++) {
        if (A[i] != nullptr) {
            if (this->Min == nullptr) {
                this->Min = A[i];
                this->Min->left = this->Min;
                this->Min->right = this->Min;
            }
            else {
                Node* Min_left = this->Min;
                Min_left->right = A[i];
                this->Min->left = A[i];
                A[i]->left = Min_left;
                A[i]->right = this->Min;
                if (A[i]->key < this->Min->key) {
                    this->Min = A[i];
                }
            }
        }
    }
}


template <class T>
void Fibonacci_Heap<T>::Insert(T x) {
    // while (id >= this->mp.size()) {
    //     this->mp.push_back(nullptr);
    // }
    Node* point = new Node(x);
    // this->mp[id] = point;
    
    // 当堆为空时
    if (this->Empty()) {
        this->Min = point;
    }
    // 当堆不为空时
    else {
        // 把新插入的结点放到Min结点的左边
        Node* tmp = this->Min->left;
        tmp->right = point;
        this->Min->left = point;
        point->left = tmp;
        point->right = this->Min;
        // 如果新结点的key比较小，那就将Min指向它
        if (point->key < this->Min->key) {
            this->Min = point;
        }    
    }
    // 结点数量+1
    this->n ++;
}


template <class T>
T Fibonacci_Heap<T>::ExtractMin() {
    // 如果是空
    if (this->Empty()) {
        return -1;
    }
    // 如果只有一个
    this->n --;
    if (this->Empty()) {
        // cout << "in " << endl;
        T ans = this->Min->key;
        // cout << "11 " << ans << endl;
        // printf("%p\n", &Min);
        delete this->Min;
        // cout << "111" << endl;
        this->Min = nullptr;
        return ans;
    }
    // 将子结点加入到根链表
    Node* tmp = this->Min->child;
    vector <Node *> child_list;
    if (tmp != nullptr) {
        do {
            child_list.push_back(tmp);
            tmp = tmp->right;
        } while (tmp != this->Min->child);
    }
    for (auto &child : child_list) {
        Node* Min_left = this->Min->left;
        child->p = nullptr;
        Min_left->right = child;
        this->Min->left = child;
        child->left = Min_left;
        child->right = this->Min;
    }
    // 将Min从根链表中删除
    Node* Min_left = this->Min->left;
    Node* Min_right = this->Min->right;
    Min_left->right = Min_right;
    Min_right->left = Min_left;
    T ans = this->Min->key;
    delete this->Min;
    this->Min = Min_left;
    // printf("%p %p\n", &Min, &Min_left);
    // cout << "left " << Min_left->key << endl;
    this->Consolidate();
    return ans;
}

template<class T>
void Fibonacci_Heap<T>::HeapUnion(Fibonacci_Heap<T> &A, Fibonacci_Heap<T> &B) {
    // 若两个堆都是空的，则合并后仍是个空堆
    if (A.Min == nullptr && B.Min == nullptr) {
        return;
    }
    // 若只有A是空的，则直接变成B
    if (A.Min == nullptr) {
        this->Min = B.Min;
    }
    // 只有B是空的，则直接变成A
    else if (B.Min == nullptr) {
        this->Min = A.Min;
    }
    // 若都不空
    else {
        // 先将Min指向A.Min
        this->Min = A.Min;
        // 先将B的结根点全部插入A的跟链表
        Node* tmp = B.Min;
        vector <Node*> root_list;
        do {
            // cout << "aaa " << tmp->key << endl;
            root_list.push_back(tmp);
            tmp = tmp->right;
        } while (tmp != B.Min);

        for (auto &root : root_list) {
            Node* Min_left = this->Min->left;
            Min_left->right = root;
            this->Min->left = root;
            root->left = Min_left;
            root->right = this->Min;
        }
        // 若B.Min比A.Min的key小，则把Min指向B.Min
        if (B.Min->key < A.Min->key) {
            this->Min = B.Min;
        }   
    }
    // 更新结点数量
    this->n = A.n + B.n;
}










