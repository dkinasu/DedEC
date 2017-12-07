/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "process.h"


// void binary_to_hex(char *dst, char *src, size_t src_size)
// {
// 	int x;

// 	for (x = 0; x < src_size; x++)
// 		sprintf(&dst[x * 2], "%.2x", (unsigned char)src[x]);
// 	return;
// }


char *hex_to_binary(char *src, size_t src_size)
{
	int x;

	char *result = (char *)malloc((src_size + 1) / 2);
	for (x = 0; x < src_size; x++){
		char c = src[x];
		unsigned int bin = (c > '9') ?
		   (tolower(c) - 'a' + 10) : (c - '0');
		if (x % 2 == 0) {
			result[x / 2] = bin << 4;
		} else {
			result[x / 2] |= bin;
		}
	}
	return result;
}


// Process Write Request: 6 cases
void Write_Process(struct traceline *T_line, int ec, int policy)
{
	long long pblk_nr = -1;
	int unique = 0;
	long long last_trace_time = 0;
	
	//keep track of total write num for all the tracefiles
	metrics[WRITES_NUM].total++;
	metrics[WRITES_BYTES].total += DATA_SIZE;
	
	// T_line->pos = T_line->Address % DATA_SIZE;
	
	
	// Print_traceline(T_line);
	// printf("-------------------------line[%lld]: Write-----------------------\n", total_line);

	struct laddr_node *laddr_node = NULL;
	struct page_node *page_node = NULL;
	struct fp_node *fp = NULL;

	sys_last_busy_time += SHA1_COST;

	laddr_node = Find_filepath(&laddr_tree, T_line->file_path);
	ladd_search_count++;

	fp = Find_fp(T_line->Sha1);
	fp_search_count++;

	


	if (laddr_node != NULL)//old file
	{
		// printf("line[%lld]: Found file: %s\n", total_line, T_line->file_path);
		ladd_hit++;

		//trying to find if the page exists
		page_node = Find_page(laddr_node->page_tree, T_line->pos);
		page_search_count++;
		
		//found page
		if (page_node != NULL)
		{
			if (fp != NULL)
			{
				w_case_3++;
				// printf("Case 3[%lld]: Same page in Same file with Same fp!\n", w_case_3);				
				total_fp_hit++;
				fp->hit++;

				// Req_Tbl[T_line->Request_index].used_nodes.insert(fp->p->node_num);

				// if(Req_Tbl[T_line->Request_index].used_nodes.size() == K)
				// 	Req_Tbl[T_line->Request_index].used_nodes.clear();
			}
			else// new fp
			{
				w_case_4++;
				// printf("Case 4[%lld] [%lld]: Same page in Same file with New fp!\n", w_case_4, total_line);
				
				fp = Init_fp_node(T_line);
				Add_fp(fp);



				// if (page_node->fp->p == NULL)
				// {
				// 	// unique = 0;
					
				// 	for (int i = 0; i < soe.data_pblk.size(); ++i)
				// 	{
				// 		if(strcmp(soe.data_pblk[i].Sha1, page_node->fp->fingerprint) == 0)
				// 		{
				// 			strcpy(soe.data_pblk[i].Sha1, page_node->fp->fingerprint)
				// 			fp->p = 
				// 		}

				// 	}
				// 	//fp->p = page_node->fp->p

				// }
				// else//update with a placed chunk
				// {
				// 	fp->p = page_node->fp->p;
				// 	// unique = 1;

				// }
				// pblk_nr = Allocate_a_pblk();
				
				// should decrease the old fp or delete it.
				fp->p = page_node->fp->p;

				if (page_node->fp->p == NULL)
				{
						for (int i = 0; i < soe.data_pblk.size(); ++i)
						{
							if(strcmp(soe.data_pblk[i].Sha1, page_node->fp->fingerprint) == 0)
							{
								soe.data_pblk[i].p = page_node;
								// Del_fp(page_node->fp);
								break;
							}

						}
				}
				// else
				// {

				// }

				
				// page_node = Init_page_node(T_line->pos, fp);
				page_node->fp = fp;
				
				// global_storage[pblk_nr]->fp = fp;
			}

		}
		else//page_node == NULL
		{
			if (fp != NULL)// hit fp
			{
				//printf("[%d] Find fp!\n", line_count);
				w_case_1++;		
				// printf("Case 1[%lld]: New page in Same file with Same fp!\n", w_case_1);
				/*1. add new page but no new pblk 2. increase ref_count 3. data_LRU_adjust*/
				fp->hit++;
				total_fp_hit++;

				page_node = Init_page_node(T_line->pos, fp);
				Add_page_node(laddr_node->page_tree, page_node);

				// Req_Tbl[T_line->Request_index].used_nodes.insert(fp->p->node_num);

				// if(Req_Tbl[T_line->Request_index].used_nodes.size() == K)
				// 	Req_Tbl[T_line->Request_index].used_nodes.clear();

				// Increase_pblk_ref_count(page_node->pblk_nr);

			}
			else// new fp
			{
				//printf("[%d] Not find fp!\n", line_count);
				w_case_2++;
				// printf("Case 2[%lld]: New page in Same file with New fp!\n", w_case_2);
				// pblk_nr = Allocate_a_pblk();
				fp = Init_fp_node(T_line);
				Add_fp(fp);
				// global_storage[pblk_nr]->fp = fp;
				// printf("Finish case 2\n");

				page_node = Init_page_node(T_line->pos, fp);
				Add_page_node(laddr_node->page_tree, page_node);
				unique = 1;
			}

		}

	}
	else//new file
	{
		laddr_node = Init_laddr_node(T_line->file_path);
		Add_laddr_node(&laddr_tree,laddr_node);	

		if (fp != NULL)//old fp
		{			
			w_case_5++;
			// printf("Case 5[%lld]: New file with old fp!\n", w_case_5);
			fp->hit++;
			total_fp_hit++;

			// // Increase_pblk_ref_count(fp->pblk_nr);
			// Req_Tbl[T_line->Request_index].used_nodes.insert(fp->p->node_num);

			// 	if(Req_Tbl[T_line->Request_index].used_nodes.size() == K)
			// 		Req_Tbl[T_line->Request_index].used_nodes.clear();

		}
		else//new fp
		{
			w_case_6++;
			// printf("Case 6[%lld]: New file with New fp!\n", w_case_6);
			// pblk_nr = Allocate_a_pblk();
			fp = Init_fp_node(T_line);
			Add_fp(fp);
			unique = 1;
			// global_storage[pblk_nr]->fp = fp;

		}

		
		page_node = Init_page_node(T_line->pos, fp);
		Add_page_node(laddr_node->page_tree, page_node);
	}


	T_line->p = page_node;

	sys_last_busy_time += ACCESS_CACHE;
	//check cache.
	if (unique == 0)
	{
		// printf("T_line->Request_index: %lld\n", T_line->Request_index);

		if (T_line->p->fp->p == NULL)
		{

			Search_fp_in_SOE(T_line, T_line->p->fp);
			// printf("Pblk not allocated!\n");
		}
		else
		{
			Req_Tbl[T_line->Request_index].used_nodes.insert(T_line->p->fp->p->node_num);
			if(Req_Tbl[T_line->Request_index].used_nodes.size() == K)
				Req_Tbl[T_line->Request_index].used_nodes.clear();
		}
		// printf("Node_num:%d\n", T_line->p->fp->p->node_num);
			// printf("!!!%ld\n", Req_Tbl[T_line->Request_index].used_nodes.size());
		
		// printf("2\n");
		// if(Req_Tbl[T_line->Request_index].used_nodes.size() == K)
		// 		Req_Tbl[T_line->Request_index].used_nodes.clear();
	}
	

	// printf("checking cache\n");
	// if(total_line > 7)
	// 	Print_cache(N_cache);
	

	if(Routine_N_cache(page_node))
	{
		N_cache_hit++;
		// printf("line[%4lld]: cache hit[%lld]\n", total_line, N_cache_hit);
		T_line->Finish_Time = sys_last_busy_time;
		Req_Tbl[T_line->Request_index].Finish_time = T_line->Finish_Time;
		Req_Tbl[T_line->Request_index].Lasting_time = Req_Tbl[T_line->Request_index].Finish_time - Req_Tbl[T_line->Request_index].Arrive_time;

		// printf("line:[%lld] RequestID: %s duplicated! Last_time[%lld]\n", total_line, T_line->RequestID,  T_line->Finish_Time - T_line->Arrive_Time);
	}
	else
	{
		// N_cache_miss++;
		// printf("line[%4lld]: cache miss[%lld]\n", total_line, N_cache_miss);
		
		if (ec == 1)
		{
			if (unique == 0)//duplicated pblk
			{
				// printf("line:[%lld] RequestID: %s duplicated!\n", total_line, T_line->RequestID);
				T_line->Finish_Time = sys_last_busy_time;
				// printf("line:[%lld] RequestID: %s duplicated! Last_time[%lld]\n", total_line, T_line->RequestID,  T_line->Finish_Time - T_line->Arrive_Time);
				Req_Tbl[T_line->Request_index].Finish_time = T_line->Finish_Time;
				Req_Tbl[T_line->Request_index].Lasting_time = Req_Tbl[T_line->Request_index].Finish_time - Req_Tbl[T_line->Request_index].Arrive_time;
								
			}
			else if(unique == 1)//new pblk
			{
				//more than time

				if (soe.full == 0)
				{
					tmp = T_line->Arrive_Time;
				}
				
				//remove timeover restriction
				if(T_line->Arrive_Time > (tmp + PLACEMENT_THRESHOLD))
				{
					// printf("[Time up: %lld %d]--> Placement!\n", (T_line->Arrive_Time - tmp), PLACEMENT_THRESHOLD);
					sys_last_busy_time += ENCODE_COST;

			   		int s_p = Placement(soe, K, M, policy);
			   		// printf("SOE is full! Start placement!\n");
			   		// Update_SOE_Time(last_trace_time);
			   		// printf("line[%lld]:error!\n", total_line);
			   		Finish_time_SOE(s_p);
			   		Print_stripe(s_p);
			   		Reset_SOE(K);

				}

				
				// printf("Push SOE\n");
				Push_SOE(*T_line);
				unique = 0;
				// printf("1\n");

			}



			if (Is_SOE_full(K))//encoding
			{

			    sys_last_busy_time += ENCODE_COST;
			    
			   	int s_p = Placement(soe, K, M, policy);
			   	// printf("SOE is full! Start placement!\n");
			   	// Update_SOE_Time(last_trace_time);
			   	Finish_time_SOE(s_p);
			   	Print_stripe(s_p);
			   	Reset_SOE(K);
			   	// Print_fps();
			}
			else
			{
				// Finish_time_SOE();

				// printf("SOE Not full\n");
			}	 
			 
			
		}
		else if(ec == 0)//no Erasure coding
		{

			//regardless of duplicates or not, go the the node and write.
			CurNode = global_storage[page_node->fp->p->pblk_num]->node_num;
			
			if (pblk_nr != -1)//new pblk
			{
				if (sys_last_busy_time >= Cluster[CurNode]->Last_Busy_Time)
				{
						Cluster[CurNode]->Last_Busy_Time = sys_last_busy_time + WRITE_LATENCY + NETWORK_COST;
				}
				else
				{
						Cluster[CurNode]->Last_Busy_Time += WRITE_LATENCY + NETWORK_COST;
				}

				T_line->Finish_Time = Cluster[CurNode]->Last_Busy_Time;
			}
			else//duplicates & cache miss: write into cache instead of swap.
			{
				//get into cache.
				T_line->Finish_Time = sys_last_busy_time;
				// sys_last_busy_time += NETWORK_COST;

			}
		}

	}

	// printf("-----------------------------------------------------------\n");	

}

void Update_R_Request_Time(struct traceline *T_line)
{

	// printf("Update_R_Request_Time!\n");
	// printf("T_line->Arrive_Time: %lld   Req_Tbl[T_line->Request_index].Arrive_time: %lld\n", T_line->Arrive_Time, Req_Tbl[T_line->Request_index].Arrive_time);
	if (Req_Tbl[T_line->Request_index].Arrive_time > T_line->Arrive_Time)
	{
		printf("Error:R_Request[%lld].Arrive_Time Wrong!\n", T_line->Request_index);
		Req_Tbl[T_line->Request_index].Arrive_time = T_line->Arrive_Time;
	}
	

	if (Req_Tbl[T_line->Request_index].Finish_time < T_line->Finish_Time)
	{
		Req_Tbl[T_line->Request_index].Finish_time = T_line->Finish_Time;
		Req_Tbl[T_line->Request_index].Lasting_time = Req_Tbl[T_line->Request_index].Finish_time - Req_Tbl[T_line->Request_index].Arrive_time;
		Req_Tbl[T_line->Request_index].straggler_chunk = T_line->trace_num;
	}
	// printf("1\n");
	
}


//Process a Read request
void Read_Process(struct traceline *T_line, int ec)
{
	// printf("-------------------------line[%lld] Read----------------------\n", total_line);
	struct laddr_node * laddr_node;

	laddr_node = Find_filepath(&laddr_tree, T_line->file_path);

	if(laddr_node == NULL)//no such file
	{
		r_case_1++;
		// printf("Read Failure![%lld]: [Failed] NO such file\n", total_line);
		return ;
	}
	else
	{
		// printf("line[%lld]: Found file: %s\n", total_line, T_line->file_path);

		struct page_node * res;

		res = Find_page(laddr_node->page_tree, T_line->pos);

		if(res == NULL)// Not find the page
		{
			r_case_3++;//have the file, but not page
			// printf("READ case 3: [Failed] NO such page in the file\n");
			return ;
		}
		else//page and file exits.
		{
			// printf("Page Found!\n");
			sys_last_busy_time += ACCESS_CACHE;

			//check cache
			if(Routine_N_cache(res))//N_cache hit
			{
				r_case_2++;
				N_cache_hit++;

				T_line->Finish_Time = sys_last_busy_time;
			}
			else//N_cache miss
			{
				r_case_4++;
				N_cache_miss++;

				CurNode = global_storage[res->fp->p->pblk_num]->node_num;

				Print_Cluster_Time();

				if (1)
				{
					printf("sys_last_busy_time: %lld  Cluster[%d]->Last_Busy_Time: %lld\n",  sys_last_busy_time, CurNode, Cluster[CurNode]->Last_Busy_Time);
				}

				if (sys_last_busy_time > Cluster[CurNode]->Last_Busy_Time)
				{
					Cluster[CurNode]->Last_Busy_Time = sys_last_busy_time + READ_LATENCY + NETWORK_COST;
				}
				else
				{
					Cluster[CurNode]->Last_Busy_Time += READ_LATENCY + NETWORK_COST;
				}

				T_line->Finish_Time = Cluster[CurNode]->Last_Busy_Time;
			}

			Update_R_Request_Time(T_line);

		}

	}

	// printf("-----------------------------------------------------------\n");

}


void Delete_Process(struct traceline *T_line, int ec)
{
	//metrics[DELETE_NUM].total++;
	//Del_file(&laddr_tree, T_line->file_path);
}


void Request(struct traceline *T_line, int ec)
{
	// printf("Into request\n");

	if (strncmp(T_line->RequestID, Last_RequestID, 20) == 0)
	{
		// printf("Old request\n");
		T_line->Request_index = CurReq.Index - 1;
		
		// printf("T_line->Request_index: %lld\n", CurReq.Index - 1);
		// printf("size of Req_tbl %lu\n", Req_Tbl.size());

		// printf("T->finish_time %lld\n", T_line->Finish_Time);
		// printf("Req_Tbl[CurReq.Index - 1].Finish_time %lld\n", Req_Tbl[CurReq.Index - 1].Finish_time);

		// if (Req_Tbl[CurReq.Index - 1].Finish_time <= T_line->Finish_Time)
		// {
		// 	// printf("1\n");
		// 	if (T_line->Arrive_Time >= Req_Tbl[CurReq.Index - 1].Finish_time)
		// 	{
		// 					Req_Tbl[CurReq.Index - 1].Lasting_time += T_line->Finish_Time - T_line->Arrive_Time;
		// 	}
		// 	else
		// 	{
		// 			Req_Tbl[CurReq.Index - 1].Lasting_time += T_line->Finish_Time - Req_Tbl[CurReq.Index - 1].Finish_time;
		// 	}

		// 	// printf("2\n");
		// 	Req_Tbl[CurReq.Index - 1].Finish_time = T_line->Finish_Time;				
		// 	Req_Tbl[CurReq.Index - 1].straggler_chunk = T_line->trace_num;	
		// }

	}
	else//new RequestID
	{
		// printf("New request\n");
		CurReq.Arrive_time = T_line->Arrive_Time;
		CurReq.Finish_time = T_line->Finish_Time;
		// CurReq.Lasting_time = CurReq.Finish_time - CurReq.Arrive_time;
		strcpy(CurReq.RequestID, T_line->RequestID);
		strcpy(CurReq.Type, T_line->Type);

		if (strcmp(T_line->Type, "write") == 0)
		{	
			/*
			if (T_line->Arrive_Time >= sys_last_busy_time - SHA1_COST)
			{
					CurReq.Lasting_time = T_line->Finish_Time - T_line->Arrive_Time;
			}
			else
			{
					CurReq.Lasting_time =T_line->Finish_Time - (sys_last_busy_time - SHA1_COST);
			}
			*/
			CurReq.W_Req_Index++;
			Count_WIO++;
		}
		else//read
		{
			// if (T_line->Arrive_Time >= sys_last_busy_time)
			// {
			// 	CurReq.Lasting_time = T_line->Finish_Time - T_line->Arrive_Time;
			// }
			// else
			// {
			// 	CurReq.Lasting_time = T_line->Finish_Time - (sys_last_busy_time);
			// }
			
			CurReq.R_Req_Index++;
			Count_RIO++;
		}
			

		Req_Tbl.push_back(CurReq);
		// printf("line[%lld]: W_Req_Index = %lld, R_Req_Index = %lld\n", total_line, Req_Tbl[CurReq.Index].W_Req_Index, Req_Tbl[CurReq.Index].R_Req_Index);
		CurReq.Index++;

		strncpy(Last_RequestID, T_line->RequestID, 20);

	}

}

void Print_Req_Tbl()
{
	std::cout << "RequestID\tType\t Index\tLasting_time\tArrive_time\tFinish_time\tR_Req_Index\tW_Req_Index\n";
	
	// printf("Cur.Index: %lld  == Req_Tbl.size: %lu\n", CurReq.Index, Req_Tbl.size());

	for (int i = 0; i < Req_Tbl.size(); i++)
	{
		if (strcmp(Req_Tbl[i].Type, "write") == 0)
		{
			W_Request_lasting_time += Req_Tbl[i].Lasting_time;
		}
		else
		{
			R_Request_lasting_time += Req_Tbl[i].Lasting_time;
		}

		std::cout << Req_Tbl[i].RequestID << "\t" << Req_Tbl[i].Type << "\t" << Req_Tbl[i].Index << "\t" << Req_Tbl[i].Lasting_time << "\t" << Req_Tbl[i].Arrive_time
			<< "\t" << Req_Tbl[i].Finish_time << "\t" << Req_Tbl[i].R_Req_Index << "\t" << Req_Tbl[i].W_Req_Index << "\n";
	}
}



void Process(char **files, int trace_start, int trace_end, struct traceline *T_line, int ec, int policy)
{
	FILE *file;
	long long empty_line = 0;
	bool done = false;
	int Op_change = 0;
	
	// int op_change = 0;
	
	char buffer[MAX_PATH_SIZE + MAX_META_SIZE];

	// printf("\n------------------------Start Processing------------------------------\n");

	for (int i = trace_start; i < trace_end; ++i)
	{
		file = fopen(files[i], "r");

		// Print out the names of the tracefiles
		// printf("%s\n",files[i]);
		
		if (file == NULL) 
		{
			// printf("Open tracefile[%d] fail %s\n", i, strerror(errno));
			continue;
		}
	
		line_count = 0;
		
		//printf("------------------------Start Processing a File-----------------------------\n");
	    while(fgets(buffer, MAX_PATH_SIZE + MAX_META_SIZE, file) != NULL && !done) 
	    {

		    if (strcmp(buffer, "\n") == 0)//strcmp verifies line has more than only \n
		    {
		        empty_line++;
		        
		  //       if(soe.full != 0)
				// {
				// 	printf("[Hit a newline]--> Placement!\n");
				// 	sys_last_busy_time += ENCODE_COST;

			 //   		int s_p = Placement(soe, K, M, policy);
			 //   		// printf("SOE is full! Start placement!\n");
			 //   		// Update_SOE_Time(last_trace_time);
			 //   		// printf("line[%lld]:error!\n", total_line);
			 //   		Finish_time_SOE();
			 //   		Print_stripe(s_p);
			 //   		Reset_SOE(K);
				// }

	    		continue;
		    }
	    	
	    	total_line++;
	    	line_count++;

	    	// if (total_line >= 504)
	    	// {
	    	// 	break;
	    	// }
	    	
	    	// printf("total_line: %lld\n", total_line);

		    if(Split_Trace(buffer, T_line) == 0)
		    {
		    	// printf("Split Trace\n");
		    	metrics[REQUESTS].total++;

		    	if(sys_last_busy_time < T_line->Arrive_Time)
		    	{
		    		sys_last_busy_time = T_line->Arrive_Time;
		    	}
				
				Request(T_line, ec);
				printf("--------------------------------line[%lld]: %s----------------------\n", total_line, T_line->Type);
				printf("line[%lld]: file_path: %s pos: %d ReqID: %s %s \n", total_line, T_line->file_path, T_line->pos, T_line->RequestID, T_line->Type);
		    	// Print_traceline(T_line);
				if(strcmp(T_line->Type, "write") == 0)
				{
					// printf("--------------------------------line[%lld]: %s----------------------\n", total_line, T_line->Type);
					// printf("line[%lld]: file_path: %s pos: %d ReqID: %s %s \n", total_line, T_line->file_path, T_line->pos, T_line->RequestID, T_line->Type);
					write_count++;
					Write_Process(T_line, ec, policy);
					
					// if(write_count == 10)
						// Print_fps();
				} 
				else if (strcmp(T_line->Type, "read") == 0)
				{

					if(soe.full != 0)
					{
						printf("Change of Operation [Read]--> Placement!\n");
						sys_last_busy_time += ENCODE_COST;

			   			int s_p = Placement(soe, K, M, policy);
			   		// printf("SOE is full! Start placement!\n");
			   		// Update_SOE_Time(last_trace_time);
			   		// printf("line[%lld]:error!\n", total_line);
			   			Finish_time_SOE(s_p);
			   			Print_stripe(s_p);
			   			Reset_SOE(K);
					}

					read_count++;
					Read_Process(T_line, ec);
				}
				else
				{
					other_count++;
				}

				// Request(T_line, ec);
				
				Clear_Traceline(T_line);	
		    }

		}

		if(soe.full != 0)
		{
			printf("[End of File]--> Placement!\n");
			sys_last_busy_time += ENCODE_COST;

			int s_p = Placement(soe, K, M, policy);
			// printf("SOE is full! Start placement!\n");
			// Update_SOE_Time(last_trace_time);
			// printf("line[%lld]:error!\n", total_line);
			Finish_time_SOE(s_p);
			Print_stripe(s_p);
			Reset_SOE(K);
		}

		printf("\nfile[%d] || linecount: %d || writecount: %d || readcount: %d || deletecount: %d || othercount: %d\n", i, line_count, write_count, read_count, delete_count, other_count);
		long long unique_fp = HASH_COUNT(fp_store);
		Count_fp();
		printf("total_fp_hit: %lld  dup_rate: %f\n", total_fp_hit, (double)(write_count-unique_fp)/(double)write_count);
		printf("Empty_line: %lld\n", empty_line);
		printf("Read Failure: [%lld]: [Failed] NO such file\n", r_case_1);
		
		Output_Result(policy);

		// total_line = 0;
		// sys_last_busy_time = 0;


		// Output()
		// Print_Req_Tbl();
		//printf("Read:  case 1: %d || case 2: %d || case 3: %d || case 4: %d\n", r_case_1, r_case_2, r_case_3, r_case_4);
		//printf("Write: case 1: %d || case 2: %d || case 3: %d || case 4: %d || case 5: %d|| case 6: %d\n", w_case_1, w_case_2, w_case_3, w_case_4, w_case_5, w_case_6);

	}
	
	

	//printf("ladd_search: %d ladd_hit: %d\n", ladd_search_count, ladd_hit);
	//printf("page_search: %d page_hit: %d\n", page_search_count, page_hit);
	//printf("total count[%lld] fp_count[%d]\n", total_line, tt);
	//printf("Write count[%d]: case 1: %d || case 2: %d || case 3: %d || case 4: %d || case 5: %d|| case 6: %d\n", write_count, w_case_1, w_case_2, w_case_3, w_case_4, w_case_5, w_case_6);
	//printf("hit: %lld, dup_rate: %f\n", total_hit, (double)total_hit/(double)write_count);
	//printf("pblk number is: %ld\n", pblk_used);

	/*
	printf("------------------------Cache Situation----------------------------------\n");
	printf("Cache size =  %ld, hit = %d, miss = %d, evict = %ld\n", cache_size, cache_hit, cache_miss, cache_evict);

	printf("------------------------Dedup Situation----------------------------------\n");
	printf("Total hit =  %ld, total_line = %d, Dedup ratio = %f%%\n", total_hit, total_line, (float)total_hit / total_line * 100);

	printf("------------------------Finish Processing------------------------------\n");
	*/
}

// void Output(char * filename)
// {
// 	char *prefix = "baseline_";

// 	ofstream r_output, w_output;
	
// 	r_output.open("Read.dat");
// 	w_output.open("Write.dat");

// 	r_output << "R_Index\tReqID\tL_time\tA_time\tF_time\n";
// 	w_output << "W_Index\tReqID\tL_time\tA_time\tF_time\n";

// 	for (int i = 0; i < CurReq.Index; i++)
// 	{
// 		if (strcmp(Req_Tbl[i].Type, "write") == 0)
// 		{
// 			W_Request_lasting_time += Req_Tbl[i].Lasting_time;
// 			r_output << Req_Tbl[i].R_Req_Index << "\t" << Req_Tbl[i].RequestID << "\t" << Req_Tbl[i].Lasting_time << "\t" << Req_Tbl[i].Arrive_time
// 			<< "\t" << Req_Tbl[i].Finish_time << "\n";
// 		}
// 		else
// 		{
// 			R_Request_lasting_time += Req_Tbl[i].Lasting_time;
// 			r_output << Req_Tbl[i].W_Req_Index << "\t" << Req_Tbl[i].RequestID << "\t" << Req_Tbl[i].Lasting_time << "\t" << Req_Tbl[i].Arrive_time
// 			<< "\t" << Req_Tbl[i].Finish_time << "\n";
// 		}

		
// 	}

// 	r_output.close();
// 	w_output.close();

// }

// void Result_Output()
// {
// 	ofstream r_output, w_output;
// 	r_output.open("Read.dat");
// 	w_output.open("Write.dat");

// 	r_output << "R_Index\tReqID\tL_time\tA_time\tF_time\t \n";
// 	w_output << "R_Index\tReqID\tType\tL_time\tA_time\tF_time\tR_Index\tW_Index\n";
	
// 	for (int i = 0; i < CurReq.Index; i++)
// 	{
// 		if (strcmp(Req_Tbl[i].Type, "write") == 0)
// 		{
// 			W_Request_lasting_time += Req_Tbl[i].Lasting_time;
// 		}
// 		else
// 		{
// 			R_Request_lasting_time += Req_Tbl[i].Lasting_time;
// 		}
// 		myfile << i << "\t" << Req_Tbl[i].RequestID << "\t" << Req_Tbl[i].Type << "\t" << Req_Tbl[i].Lasting_time << "\t" << Req_Tbl[i].Arrive_time
// 			<< "\t" << Req_Tbl[i].Finish_time << "\t" << Req_Tbl[i].R_Req_Index << "\t" << Req_Tbl[i].W_Req_Index << "\n";
// 	}
// 	myfile.close();

// }


