/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   process.h
 * Author: dkinasu
 *
 * Created on October 23, 2017, 10:54 PM
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"
#include "storage.h"
#include "trace.h"
#include "cache.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <iostream>
#include <fstream>

#define BASELINE 0
#define ERASURE  1

#define READ_LATENCY 60  //us
#define WRITE_LATENCY 200
#define ACCESS_CACHE 1 // cache miss time is overshadowed by the serving.
#define NETWORK_COST 3.2// us
#define RECON_COST 500    //reconstruct the whole file
#define DECODE_COST 700  //decoding one chunk
#define SHA1_COST 80
#define ENCODE_COST 250 //encoding a stripe 
#define PLACEMENT_THRESHOLD   15000 //Time threshold for wait

using namespace std;

void Process(char **files, int trace_start, int trace_end, struct traceline *T_line, int baseline, int policy);
void Write_Process(struct traceline *T_line, int baseline, int policy);
void Read_Process(struct traceline *T_line, int baseline);
void Delete_Process(struct traceline *T_line, int baseline);
void Request(struct traceline *T_line, int ec);
void Print_Req_Tbl();
void Output(char * filename);
void Update_R_Request_Time(struct traceline *T_line);

// define 6 situations of write
extern long long w_case_1;
extern long long w_case_2;
extern long long w_case_3;
extern long long w_case_4;
extern long long w_case_5;
extern long long w_case_6;

// define 6 situations of read
extern long long r_case_1;
extern long long r_case_2;
extern long long r_case_3;
extern long long r_case_4;
extern long long r_case_5;
extern long long r_case_6;

extern long long tmp;

//char *hex_to_binary(char *src, size_t src_size);


#endif /* PROCESS_H */

