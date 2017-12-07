/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "trace.h"


void Init_CurRequest()
{
	CurReq.Index = 0;
	CurReq.R_Req_Index = 0;
	CurReq.W_Req_Index = 0;
	CurReq.Arrive_time = 0;
	CurReq.Finish_time = 0;
	CurReq.Lasting_time = 0;
	CurReq.straggler_chunk = 0;
}

void Init_LastReqestID()
{
	strcpy(Last_RequestID, "00000000000000000000");
}

/*
//Parse each traceline and load into &Tline
int TraceLine_Parse(char * buffer, struct traceline *T_line)
{
    //printf("------------------------Parsing One TraceLine----------------------------\n");
	int i = 0;
	char *result = NULL;

	//strsep::  seperate the str with the deliminater ","
	//result is NULL
	//get every single part of the traceline, and put them into replay_node
	while((result = strsep(&buffer, ",")) != NULL) {
		
		switch(i) {
		case 0:
			//get the time_stamp, and put into the &replay_node->time_stamp
			sscanf(result, "%llu", &T_line->time_stamp);
			break;
		case 1:
			T_line->file_path = (char *)malloc(strlen(result) + 1);
			strncpy(T_line->file_path, result, strlen(result));
			T_line->file_path[strlen(result)] = '\0';
			break;
		case 2:
			sscanf(result, "%lu", &T_line->inode_no);
			break;
		case 3:
			sscanf(result, "%lu", &T_line->data_size);
			break;
		case 4:
			sscanf(result, "%ld", &T_line->pos);
			break;
		case 5:
			//this decides read or write
			sscanf(result, "%d", &T_line->rw);
			break;
		case 6:
			strcpy(T_line->pname, result);
			break;
		case 7:
			if ((strlen(result)) != 41) 
			{
				printf("result: %lu\n", strlen(result));
				T_line->databuf = NULL;
				return 0;
			} 
			else 
			{
				
				T_line->databuf = (char *)malloc(sizeof(char) * (SIZE_PER_FPRECORD) + 1);
				
				if (!T_line->databuf) 
				{
					printf("low memory on host!\n");
					exit(-1);
				}

				strncpy(T_line->databuf, result, SIZE_PER_FPRECORD);
				
				if (SIZE_PER_FPRECORD != (strlen(result) - 1)) 
				{
					
					metrics[PAGE_SIZE_WRONG].total ++;
				}
				
				T_line->databuf[SIZE_PER_FPRECORD] = '\0';
			}

			break;
		}

		i++;
	}
	return 1;

    //printf("------------------------Finish Parsing----------------------------------------\n");
}
*/

long long int str2num(char str1[], char str2[])
{
	long long int num = 0, m = 1, q = 0;
	int leng = strlen(str2);
	for (q = 0; q < leng; q++)
		m = m * 10;
	num = atoi(str2) + atoi(str1)*m;
	return num;
}



//Parse each traceline and load into &Tline
int Split_Trace(char * buffer, struct traceline *T_line)
{	
	char Time[200];
	char *time1;
	char *time2;
	char diskname[10];
	char scsiID[64], requestID[64], bioID[64], bi_sector[64], len[64], size[64], sector_i[64], type[64], sha1[SHA1SIZE], crc32[64];//sha1ÊÇ59Î»µÄchar
	//	int len_i, size_i;
	sscanf(buffer, "[ %s%s%s%s%s%s%s%s%s%s%s%s", Time, diskname, scsiID, requestID, bioID, bi_sector, len, size, sector_i, type, sha1, crc32);
	//test4   11643: test3
	if(total_line == 1)
	{
		//printf("%s\n", Time);
	}
	
	char seps[] = ".]";
	time1 = strtok(Time, seps);
	time2 = strtok(NULL, seps);
	
	// if(total_line == 1)
	// {
	// 	printf("%s\n%s\n", time1, time2);
	// }
		
	int t1 = atoi(time1) - 3420;
	snprintf(time1, 200, "%d", t1);

	long long int time = str2num(time1, time2); // the unit of time is us.
	//long long int address = str2num(bioID, sector_i) % MAX_ADDR; 
	long long address = str2num(bioID, sector_i) % 2000000;
	

	T_line->Address = address;
	T_line->Arrive_Time = time;
	strncpy(T_line->Sha1, sha1, SHA1SIZE);
	strncpy(T_line->RequestID, requestID, 21);
	strncpy(T_line->file_path, bi_sector, 64);
	// printf("file_path: %s  bio_sector: %s\n", T_line->file_path,  bi_sector);
	strncpy(T_line->Type, type, 16);
	T_line->p = NULL;
	T_line->Request_index = CurReq.Index;
	T_line->trace_num = total_line -1;
	T_line->pos = T_line->Address % 1000;
	return 0;

    //printf("------------------------Finish Parsing----------------------------------------\n");
}


//Clear the Tline
int Clear_Traceline(struct traceline *T_line)
{
	// printf("Clear Traceline\n");
	T_line->Arrive_Time = 0;
	T_line->Finish_Time = 0;
	T_line->Address = 0;
	T_line->Hit = 0;
	T_line->pos = 0;

	memset(T_line->Sha1, '\0', 64);
	memset(T_line->RequestID, '\0', 21);
	memset(T_line->Type, '\0', 16);
	memset(T_line->file_path, '\0', 64);
}

//print_trace
void Print_traceline(struct traceline *T_line)
{
	// printf("T_line->Arrive_Time: %lld\n", T_line->Arrive_Time);
	// printf("T_line->Finish_Time: %lld\n", T_line->Finish_Time);
	// printf("T_line->Sha1: %s\n", T_line->Sha1);
	// printf("T_line->RequestID: %s\n", T_line->RequestID);
	// printf("T_line->Address: %lld\n", T_line->Address);
	// printf("T_line->Type: %s\n", T_line->Type);
	// printf("T_line->Hit: %d\n", T_line->Hit);
	printf("line[%lld]: %lld %lld %s %s %lld %s %d %lld\n", total_line, T_line->Arrive_Time, T_line->Finish_Time, T_line->Sha1, T_line->RequestID, T_line->Address, T_line->Type, T_line->pos, T_line->Request_index);
}

//Parse the args of input
int Argv_Parse(int argc, char ** argv, char *file_prefix)
{  
    //printf("------------------------Doing Argv_Parsing----------------------------\n");
	
	if (argc < 9) {
		printf("Do not have enough parameters!\n");
		printf("Usage: ./main [tracefolder_path/] [(policy)1|2] [start_num] [end_num] [result_file] [node_num] [max_blk_num] [maxFp_num] [cache_size]\n");
		return -1;
	}
                   
    DIR *sp;

	if((sp = opendir(argv[1])) == 0) 
    {
		perror("fail to open dir");
		return -1;
	}
    closedir(sp);

	//assign args
	trace_start = atoi(argv[2]);
    trace_end = atoi(argv[3]);
    N_num = atoi(argv[5]);
    max_fp_num = atoi(argv[6]);
    max_blk_num = atoi(argv[7]);
    cache_size = atoi(argv[8]);
    
    // form the legal tracefile address
    files = (char **)malloc((trace_end - trace_start)*sizeof(char *));
    
    for(int i = trace_start; i < trace_end; i++)
    {
    	files[i] = (char *)malloc(500 * sizeof(char));
    	memset(files[i], '\0', 500);
    	sprintf(files[i], "%s%s%d", argv[1], file_prefix, i);
    	//printf("file[%d]: %s\n", i, files[i]);
   	}
    
    //printf("------------------------Finish Argv_Parsing----------------------------\n");

};

void Print_Req_used_nodes(long long index)
{
	std::set<long long>::iterator si;
	
	printf("Used_nodes: ");

	for (si = Req_Tbl[index].used_nodes.begin(); si != Req_Tbl[index].used_nodes.end(); si++)
	{
		printf("%lld ", (*si));
	}

	printf("\n");
}




//write the results to the place of *result
void Output_Result(int policy)
{
	// printf("1\n");
	char *result_prefix = (char *)".res";
	
	char *R_output = (char *)malloc(500 * sizeof(char));
	char *W_output = (char *)malloc(500 * sizeof(char));


	sprintf(R_output, "%s%d%s", "Read", policy, result_prefix);
	sprintf(W_output, "%s%d%s", "Write", policy, result_prefix);

    FILE *R_out, *W_out;
    
    char result_buf[4096];
    int i;

    //open result
    R_out = fopen(R_output, "wb+");
    W_out = fopen(W_output, "wb+");
  
    //Output the result to result!
    // sprintf(result_buf, "***********Policy[%d]__Read__*************\n", policy); 
    // result_buf[strlen(result_buf)] = '\0';
    // fwrite(result_buf, 1, strlen(result_buf), R_out);
    // sprintf(result_buf, "%-8s%10s%10s%10s%10s\n", "R_Index", "ReqID", "L_time", "A_time", "F_time"); 
    // result_buf[strlen(result_buf)] = '\0';
    // fwrite(result_buf, 1, strlen(result_buf), R_out);


    // sprintf(result_buf, "***********Policy[%d]__Write__*************\n", policy); 
    // result_buf[strlen(result_buf)] = '\0';
    // fwrite(result_buf, 1, strlen(result_buf), W_out);
    // sprintf(result_buf, "%-8s%10s%10s%10s%10s\n", "W_Index", "ReqID", "L_time", "A_time", "F_time"); 
    // result_buf[strlen(result_buf)] = '\0';
    // fwrite(result_buf, 1, strlen(result_buf), W_out);



    for (int i = 0; i < CurReq.Index; i++)
	{
		if (strcmp(Req_Tbl[i].Type, "write") == 0)
		{
			W_Request_lasting_time += Req_Tbl[i].Lasting_time;
			sprintf(result_buf, "%-8lld%10s%10lld%10lld%10lld\n", Req_Tbl[i].W_Req_Index, Req_Tbl[i].RequestID, Req_Tbl[i].Lasting_time, Req_Tbl[i].Arrive_time, Req_Tbl[i].Finish_time);
			result_buf[strlen(result_buf)] = '\0';
   			fwrite(result_buf, 1, strlen(result_buf), W_out); 

		}
		else
		{
			R_Request_lasting_time += Req_Tbl[i].Lasting_time;

			sprintf(result_buf, "%-8lld%10s%10lld%10lld%10lld\n", Req_Tbl[i].R_Req_Index, Req_Tbl[i].RequestID, Req_Tbl[i].Lasting_time, Req_Tbl[i].Arrive_time, Req_Tbl[i].Finish_time);
			result_buf[strlen(result_buf)] = '\0';
   			fwrite(result_buf, 1, strlen(result_buf), R_out);	
		}
		
	}


    fclose(R_out);
    fclose(W_out);

    // printf("1\n");

}



void Create_Default_Setting(char *** p)
{
    (*p) = (char **)malloc(9 * sizeof(char *));


    (*p)[0] = (char *)"./main";
    (*p)[1] = (char *)"/home/dkinasu/Documents/Research/Tracefile/";
    (*p)[2] = (char *)"0";
    (*p)[3] = (char *)"3";
    (*p)[4] = (char *)"1.txt";
    (*p)[5] = (char *)"1";
    (*p)[6] = (char *)"5000";
    (*p)[7] = (char *)"200000";
    (*p)[8] = (char *)"500000"; /*500000 * 4KB = 2GB*/

}
