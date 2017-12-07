#include "trace.h"
#include "process.h"
#include "memory.h"
#include "storage.h"
#include "cache.h"

#define SHA1_COST 80


#define MAXTRACELINE_W 1000//traceÖÐµÄÐ´tracelineÊýÄ¿ 52026 write_2 16 new 88261
#define MAXTRACELINE_TEST 6000

#define MAX_ADDR 2000000
#define CHANNEL_NUM 12

#define THRESHOLD 2000 // µ¥Î»us, ¼´1000us=1ms





// trace.h
char **files;/* file_path for all the tracefiles*/
int trace_start;
int trace_end; 
int N_num;
long long max_fp_num;
long long max_blk_num = 200000;
long long cache_size = 0;


struct traceline T_line;
char **default_argv = NULL;
struct metric metrics[METRICS_NUM];
char *file_prefix = (char *)"mobi.trace.";

long long total_line; 
int line_count, write_count, read_count, delete_count, other_count;

struct Request CurReq;
std::vector<struct Request> Req_Tbl(0);
char Last_RequestID[21];

long long Count_WIO = 0;
long long Count_RIO = 0;
long long W_Request_lasting_time = 0;
long long R_Request_lasting_time = 0;





// stroage.h
struct Node *Cluster[K+M];
long long sys_last_busy_time = 0;
long long global_time = 0;
int CurNode = -1;

long long pblk_used = 0;
long long pblk_serial = 0;
std::vector<struct pblk_node*> global_storage(0);


struct SOE soe; 

std::deque<long long> assign_set(0);

long long max_stripe_num;
long long stripe_num = 0;
long long used_stripe_num = 0;
std::vector<struct stripe> stripe_tbl(0);





// memory.h
struct fp_node * fp_store = NULL; /* hashtable for fp_store */
unsigned fp_count;
//struct list_head lru_fp_list;
long long total_fp_hit = 0;

int ladd_search_count;
int page_search_count;
int fp_search_count;
int ladd_hit;
int page_hit;
int fp_hit;

long long fp_in_N_cache_count = 0;
long long fp_in_F_cache_count = 0;


struct rb_root laddr_tree;


// cache.h 
std::list< struct fp_node *> N_cache(0);
std::list< struct fp_node *> F_cache(0);

long long N_cache_size = 0;
long long F_cache_size = 0;
long long N_cache_hit = 0;
long long F_cache_hit = 0;
long long N_cache_miss = 0;
long long F_cache_miss = 0;


// process.h 
// define 6 situations of write
long long w_case_1 = 0;
long long w_case_2 = 0;
long long w_case_3 = 0;
long long w_case_4 = 0;
long long w_case_5 = 0;
long long w_case_6 = 0;

// // define 6 situations of read
long long r_case_1 = 0;
long long r_case_2 = 0;
long long r_case_3 = 0;
long long r_case_4 = 0;
long long r_case_5 = 0;
long long r_case_6 = 0;

long long tmp = 0;

// //map<long, long> ref_statis;

// struct ref_node *ref_tbl = NULL;
