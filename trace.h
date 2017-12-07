/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   trace.h
 * Author: dkinasu
 *
 * Created on October 19, 2017, 11:11 AM
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <cstring>
#include <set>
#include <deque>

// #include "memory.h"
// #include "storage.h"

#ifndef TRACE_H
#define TRACE_H

// Predefine
#define SHA1SIZE 64
#define MAX_PATH_SIZE 5120
#define MAX_META_SIZE 100000

// Predefine for the metric
#define METRICS_NUM 7
#define REQUESTS 0
#define WRITES_NUM 1
#define WRITES_BYTES 2
#define DELETE_NUM 3
#define FINGERPRINT_NUM 4

extern long long Count_WIO;
extern long long Count_RIO;
#define FINGERPRINT_DELETE_NUM 5
#define PAGE_SIZE_WRONG 6 

// Global paras from Trace
// Input paras
extern char ** files;
extern int trace_start; 
extern int trace_end;  
extern int N_num;
extern long long max_fp_num;
extern long long max_blk_num;
extern long long cache_size;


extern struct metric metrics[METRICS_NUM];
extern int line_count, write_count, read_count, delete_count, other_count;
extern long long total_line; 
extern long long Count_WIO;
extern long long Count_RIO;
extern long long W_Request_lasting_time;
extern long long R_Request_lasting_time;


//a struct for each line of trace
struct traceline {
	/*
	char *file_path;
	long unsigned int data_size;
	long int pos;
	long unsigned int inode_no;
	char *databuf;
	int rw;
	char pname[TASK_COMM_LEN];
	unsigned long long time_stamp;
	*/
	long long trace_num;
	long long Arrive_Time;
	long long Finish_Time;
	char Sha1[64];
	char RequestID[21];
	long long Request_index;
	char file_path[64];
	long long Address;
	int pos;
	char Type[16];
	int Hit;
	struct page_node *p;
	std::deque<long long> dup;//to contain the request index of the duplicated chunks.
};

int Split_Trace(char * buffer, struct traceline *T_line);
int TraceLine_Parse(char * buffer, struct traceline *T_line);
int Clear_Traceline(struct traceline *T_line);
void Print_traceline(struct traceline *T_line);



//define a metric for output, like total_write_num.
struct metric {
	char *mtrk_name;
	long long unsigned total;
	long long unsigned fails;
	long long unsigned seek_fails;
	long long unsigned open_fails;
};

/**/
struct Request
{
	long long Index;//
	long long R_Req_Index;
	long long W_Req_Index;
	long long Arrive_time;
	long long Finish_time;
	long long Lasting_time;
	char RequestID[21];
	char Type[16];
	int straggler_chunk;
	std::set<long long>used_nodes;
};

extern struct Request CurReq;
extern std::vector<struct Request> Req_Tbl;
extern char Last_RequestID[21];

void Init_CurRequest();
void Init_LastReqestID();
void Print_Req_used_nodes(long long index);






//void Output_Result(char *result);
int Argv_Parse(int argc, char ** argv, char *file_prefix);

// create a default setting
void Create_Default_Setting(char *** p);

void Output_Result(int policy);
#endif /* TRACE_H */

