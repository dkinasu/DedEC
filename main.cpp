
#include <cstdio>
#include <iostream>
#include <ctime>
#include <stdlib.h>

#include "para.h"

using namespace std;


void test()
{
    int k = 0;

    for (int j = 0; j < 5; ++j)
    {
        // printf("1\n");
        assign_set.clear();
        for (int i = 0; i < K + M; ++i)
        {
            // printf("2\n");
            assign_set.push_back(k);
            k++;
            printf("%lld ", assign_set.at(i));
        }

        printf("\n");
        // Random();

        // DA_Placement(j, 1);
        // Dual_Aware(j, 1);
        // Baseline_RR(j, 1);
    }
    
}




int main(int argc, char** argv) 
{
    
    Argv_Parse(argc, argv, file_prefix);

    // char ofile[] = "baseline.txt";
    printf("N_num = %d\n", N_num);
    // Init_Cluster(K+M);
    // Init_CurRequest();
    // Init_LastReqestID();
    // Set_cache_size(N_CACHE, CACHE_SIZE);

    Init(K+M, 0);

    int policy = atoi(argv[9]);

    // test();
    if(policy == 0)
    {
         printf("------------Baseline_Placement!----------\n");
            
    }
    else if(policy == 1)
    {
        printf("--------------Random_Placement!----------\n");
         
    }
    else if(policy == 2)
        printf("--------------DA_Placement!----------\n");
    else
    {
        printf("Error:[policy is out of range]\n");
        return 1;
    }
    
    Process(files, trace_start, trace_end, &T_line, ERASURE, policy);


    // /()
    // for (int i = 0; i < 10; ++i)
    // {
    // 	struct page_node *p = Init_page_node(i, Allocate_a_pblk());
    // 	Routine_N_cache(p);
    // }    
    
    // Print_cache(N_cache);
    // printf("%lu\n", global_storage.size());
    

    return 0;
}