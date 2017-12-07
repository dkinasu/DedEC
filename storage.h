/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   storage.h
 * Author: dkinasu
 *
 * Created on October 23, 2017, 4:47 PM
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <cstdlib>
#include <cstdio>
#include "memory.h"
#include "uthash.h"
#include "process.h"

#include <vector>
#include <map>
#include <algorithm>
#include <deque>

#define PBLK_SIZE 20971520
#define MAX_BLK_NUM 1000000 //4GB 
#define K 4
#define M 2
#define B_RR 0
#define RANDOM 1
#define DA 2

typedef std::vector<int> IVec;
typedef std::vector<long> LVec;


// /*get the fp_node of the pblk*/
// #define pblk_get_addr(pblk_list, pblock)	\
// 					(struct fp_node *)((pblk_list)[(pblock)]->fp_node)
// set the fp_node of the pblk
// #define pblk_set_addr(pblk_list, pblock, node)	\
// 					(((pblk_list)[(pblock)])->fp = (void *)node)				


struct Node
{
	int num;
	long long Last_Busy_Time;
	long long pblk_used;
	unsigned int Physical_Address;//ÏÂÒ»¸ö¸ÃÍ¨µÀ¿ÉÓÃµÄµØÖ·±àºÅ
	int failed;
};

extern struct Node *Cluster[K+M];
extern int CurNode;

extern long long sys_last_busy_time;
extern long long global_time;


void Init_Cluster(int node_num);
void Print_Cluster_Time();

/* define physical block_node struct */
struct pblk_node
{
	int ref_count;
	struct fp_node* fp;
	
	int node_num;
	int stripe_num;
	int corrupted;
	int pblk_num;
	int parity;
};

extern std::vector<struct pblk_node*> global_storage;

extern long long pblk_serial;
extern long long pblk_used;
extern std::deque<long long> assign_set;

long long Allocate_a_pblk();

int Pblk_is_free(unsigned pblk_nr);
long long Mark_pblk_free(unsigned pblk_nr);

void Print_pblk(int pblk_num);

void Increase_pblk_ref_count(unsigned pblk_nr);
void Decrease_pblk_ref_count(unsigned pblk_nr);
void Baseline_RR(long long sp, int stride);
void Random(long long sp);
void Dual_Aware(long long sp, int stride);


extern struct cluster_node *node;

struct SOE
{
	std::vector<struct traceline > data_pblk;
	// long *data_pblk;
	int full;
};

extern struct SOE soe; 

void Init_SOE(int k);
void Push_SOE(struct traceline p);
int Is_SOE_full(int k);
void Reset_SOE(int k);
void Print_SOE();
void Finish_time_SOE(int sp);
void Update_SOE_Time(long long &last);
int Search_fp_in_SOE(struct traceline *T_line, struct fp_node *s);

struct stripe
{
	long long stripe_num;
	int length;
	std::vector <long long>data;
	std::vector <long long>parity;
	int corrupted;
	int used;
};

// int Is_stripe_free(int n);
void Init_a_stripe(struct stripe &s, int k, int m);
void Encode_a_stripe(int stripe_num, int m);
// long Assign_parity(int k);
long long Placement(struct SOE soe, int k, int m, int policy);
void Print_stripe(long stripe_num);
// void Init_stripe_tbl(int max_stripe_num);

extern long long max_stripe_num;
extern long long stripe_num;
extern long long used_stripe_num;
extern std::vector<struct stripe> stripe_tbl;

/*




extern long pblk_used_before_ec;



struct ref_node
{
	int ref_count;
	unsigned int pblk_nr;
	UT_hash_handle hh; /* makes this structure hashable */
// };

// extern struct ref_node *ref_tbl;
// extern std::map<long, long> ref_statis;

// void Create_ref_statistics(std::map<long, long> &ref_statis);
// int Threshold(int i);

// void Encode(LVec *p, int k, int m);
// void Diff_EC(int group_num, int base_k, int base_m);
// void Print_vec(LVec &p);

void Init(int node_num, int cache_size_N);
void Destory();
void Destroy_Cluster();
// Init_Cluster(K+M);
    // Init_CurRequest();
    // Init_LastReqestID();
    // Set_cache_size(N_CACHE, CACHE_SIZE);


#endif /* STORAGE_H */

