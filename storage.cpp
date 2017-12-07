/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "storage.h"

void Init_Cluster(int node_num)
{
	for (int i = 0; i < node_num; ++i)
	{
		Cluster[i] = new Node;

		if(Cluster[i] != NULL)
		{
			Cluster[i]->num = 0; 
			Cluster[i]->Last_Busy_Time = 0;
			Cluster[i]->pblk_used = 0;
			Cluster[i]->Physical_Address = i * MAX_BLK_NUM;//ÏÂÒ»¸ö¸ÃÍ¨µÀ¿ÉÓÃµÄµØÖ·±àºÅ
			Cluster[i]->failed = 0;
		}		
	}
}


void Destroy_Cluster()
{
	for (int i = 0; i < 8; ++i)
	{
		free(Cluster[i]);
	}
}


long long Allocate_a_pblk()
{
	if(pblk_used < MAX_BLK_NUM * N_num -1)
	{
		if(pblk_serial == MAX_BLK_NUM * N_num -1)
		{
			pblk_serial = 0;
		}

		struct pblk_node* p = new pblk_node;		

		global_storage.push_back(p);

		global_storage[pblk_serial]->ref_count = 1;
		global_storage[pblk_serial]->node_num = pblk_serial % N_num;
		global_storage[pblk_serial]->parity = 0;


		global_storage[pblk_serial]->fp = NULL;
		// global_storage[pblk_serial]->in_N_cache = 0; 
		// global_storage[pblk_serial]->in_F_cache = 0; 
		global_storage[pblk_serial]->stripe_num = -1;
		global_storage[pblk_serial]->corrupted = 0;
		global_storage[pblk_serial]->pblk_num = pblk_serial;
		
		pblk_used++;
		
		pblk_serial++;
		// printf("%lld \n", pblk_serial);
		//used_pblk.push_back(pblk_serial);
		
		return pblk_serial-1;
			
	}
	else
	{
		printf("Storage full: No space!\n");
		return -1;

	}
}


void Destroy_storage()
{
	for (int i = 0; i < global_storage.size(); ++i)
	{
		free(global_storage[i]->fp);
		free(global_storage[i]);
	}

	global_storage.clear();
}





void Increase_pblk_ref_count(unsigned pblk_nr)
{
	global_storage[pblk_nr]->ref_count++;
}






// void Destroy_storage(int max_pblk_num)
// {

// }


int Pblk_is_free(unsigned pblk_nr)
{
	return (global_storage[pblk_nr]->ref_count == 0);
}


long long Mark_pblk_free(unsigned pblk_nr)
{
	global_storage[pblk_nr]->ref_count = 0;
	global_storage[pblk_nr]->fp = NULL;
	// global_storage[pblk_nr]->in_N_cache = 0;
	// global_storage[pblk_nr]->in_F_cache = 0;
	
	pblk_used--;
	
	return pblk_used;
}





void Print_pblk(int pblk_num)
{
	printf("1");
	if(global_storage[pblk_num] == NULL)
	{
		printf("pblk[%d] does not exist!\n", pblk_num);
	}
	else
	{
		struct fp_node* fp;
		int corrupted;
		int parity;
		printf("[num: %d], [node_num: %d], [str_num: %d], [ref: %d], \
			[corrupted: %d], [parity: %d], [fp: %s]", \
			pblk_num, global_storage[pblk_num]->node_num, global_storage[pblk_num]->stripe_num, global_storage[pblk_num]->ref_count, \
			global_storage[pblk_num]->corrupted, global_storage[pblk_num]->parity, global_storage[pblk_num]->fp->fingerprint);
	}
	
}

void Update_SOE_Time(long long &last)
{
	int temp;

	temp = global_storage[soe.data_pblk[soe.full-1].p->fp->p->pblk_num]->node_num;

	// printf("last time: %lld\n", last);

	if (sys_last_busy_time >= Cluster[temp]->Last_Busy_Time)
	{
		Cluster[temp]->Last_Busy_Time = sys_last_busy_time + WRITE_LATENCY + NETWORK_COST + ENCODE_COST;
	}
	else
	{
		Cluster[temp]->Last_Busy_Time += WRITE_LATENCY + NETWORK_COST + ENCODE_COST;
	}

	if(Cluster[temp]->Last_Busy_Time >= last)
	{
		soe.data_pblk[soe.full-1].Finish_Time = Cluster[temp]->Last_Busy_Time;
		last = soe.data_pblk[soe.full-1].Finish_Time;
	}
	else
		soe.data_pblk[soe.full-1].Finish_Time = last;

	//Update Req_Tble
	if (Req_Tbl[soe.data_pblk[soe.full-1].Request_index].Finish_time <= soe.data_pblk[soe.full-1].Finish_Time)
	{
		// printf("1\n");
		if (soe.data_pblk[soe.full-1].Arrive_Time >= Req_Tbl[soe.data_pblk[soe.full-1].Request_index].Finish_time)
		{
			Req_Tbl[soe.data_pblk[soe.full-1].Request_index].Lasting_time += soe.data_pblk[soe.full-1].Finish_Time - soe.data_pblk[soe.full-1].Arrive_Time;
		}
		else
		{
			// printf("2\n");
			Req_Tbl[soe.data_pblk[soe.full-1].Request_index].Lasting_time += soe.data_pblk[soe.full-1].Finish_Time - Req_Tbl[soe.data_pblk[soe.full-1].Request_index].Finish_time;
		}

			// printf("3\n");
			Req_Tbl[soe.data_pblk[soe.full-1].Request_index].Finish_time = soe.data_pblk[soe.full-1].Finish_Time;				
			Req_Tbl[soe.data_pblk[soe.full-1].Request_index].straggler_chunk = soe.data_pblk[soe.full-1].trace_num;
	}

}

void Finish_time_SOE(int sp)
{
	int temp;
	int blk_in_stripe = 0;

	//change the time of parity nodes.
	for (int i = 0; i < M; ++i)
	{
		temp = global_storage[stripe_tbl[sp].parity[i]]->node_num;

		if (sys_last_busy_time >= Cluster[temp]->Last_Busy_Time)
		{
			Cluster[temp]->Last_Busy_Time = sys_last_busy_time + WRITE_LATENCY + NETWORK_COST;
		}
		else
		{
			blk_in_stripe++;
			// printf("Blocking!!!![%d]\n", blk_in_stripe);
			Cluster[temp]->Last_Busy_Time += WRITE_LATENCY + NETWORK_COST;
		}
	}


	for (int i = 0; i < soe.data_pblk.size(); ++i)
	{
		temp = global_storage[soe.data_pblk[i].p->fp->p->pblk_num]->node_num;

		// printf("Cluster[%d]->Last_Busy_Time: %lld sys_last_busy_time:%lld\n", temp, Cluster[temp]->Last_Busy_Time, sys_last_busy_time);

		if (sys_last_busy_time >= Cluster[temp]->Last_Busy_Time)
		{
			Cluster[temp]->Last_Busy_Time = sys_last_busy_time + WRITE_LATENCY + NETWORK_COST;
		}
		else
		{
			blk_in_stripe++;
			// printf("Blocking!!!![%d]\n", blk_in_stripe);
			Cluster[temp]->Last_Busy_Time += WRITE_LATENCY + NETWORK_COST;
		}

		soe.data_pblk[i].Finish_Time = Cluster[temp]->Last_Busy_Time;

		//Update Request
		// printf("soe.data_pblk[i].Request_index: %lld\n", soe.data_pblk[i].Request_index);

		// printf("Req_Tbl[%lld].Finish_time: %lld  soe.data_pblk[%d].Finish_Time: %lld\n", soe.data_pblk[i].Request_index, Req_Tbl[soe.data_pblk[i].Request_index].Finish_time, i, soe.data_pblk[i].Finish_Time);

		if (Req_Tbl[soe.data_pblk[i].Request_index].Finish_time <= soe.data_pblk[i].Finish_Time)
		{
			// printf("1\n");
			if (soe.data_pblk[i].Arrive_Time >= Req_Tbl[soe.data_pblk[i].Request_index].Finish_time)
			{
				Req_Tbl[soe.data_pblk[i].Request_index].Lasting_time += soe.data_pblk[i].Finish_Time - soe.data_pblk[i].Arrive_Time;
			}
			else
			{
				// printf("2\n");
				Req_Tbl[soe.data_pblk[i].Request_index].Lasting_time += soe.data_pblk[i].Finish_Time - Req_Tbl[soe.data_pblk[i].Request_index].Finish_time;
			}

			// printf("3\n");
			Req_Tbl[soe.data_pblk[i].Request_index].Finish_time = soe.data_pblk[i].Finish_Time;				
			Req_Tbl[soe.data_pblk[i].Request_index].straggler_chunk = soe.data_pblk[i].trace_num;
		}
	}

	// Print_Cluster_Time();
}

// /* delete a pblk node by decreasing its ref_count*/
// void Decrease_pblk_ref_count(unsigned pblk_nr)
// {
// 	if (storage[pblk_nr]->ref_count == 0)
// 	{
// 		printf("Error: This pblk[%d] is not allocated\n", pblk_nr);
// 	}
// 	else
// 	{
// 		storage[pblk_nr]->ref_count--;

// 		if(storage[pblk_nr]->ref_count == 0)
// 		{
// 			LVec::iterator it = std::find(used_pblk.begin(), used_pblk.end(), pblk_nr);
			
// 			if(it != used_pblk.end())
// 			{
// 				used_pblk.erase(it);
// 			}

// 			/*1. mark this pblk unused 2. release fp_node */
// 			if (Pblk_is_in_mem(pblk_nr))
// 			{
// 				//Page_lru_del()
// 				storage[pblk_nr]->in_mem = 0;
// 			}
		
// 			Del_fp(storage[pblk_nr]->fp);
			
// 			Mark_pblk_free(pblk_nr);
// 		}
// 	}
	
// 	return;

// }




// void Init_stripe_tbl(int max_stripe_num)
// {
// 	// stripe_tbl = (struct stripe *)malloc(max_stripe_num * sizeof(struct stripe));

// 	// if(stripe_tbl == NULL)
// 	// {
// 	// 	printf("Stripe_tbl Init [Failed]\n");
// 	// }

// 	// for (int i = 0; i < max_stripe_num; ++i)
// 	// {
// 	// 	stripe_tbl[i].stripe_num = i;
// 	// 	stripe_tbl[i].length = 0;
// 	// 	stripe_tbl[i].data = NULL;
// 	// 	stripe_tbl[i].parity = NULL;

// 	// 	stripe_tbl[i].corrupted = 0;
// 	// 	stripe_tbl[i].used = 0;
// 	// }

// }

// /*Init a pblk_node*/
// long Find_a_free_stripe()
// {
// 	//printf("Finding a free stripe!\n");
// 	if(used_stripe_num < max_stripe_num -1)
// 	{
// 		for (int i = 0; i < max_stripe_num; ++i)
// 		{
// 			if(Is_stripe_free(i))
// 			{			
// 				return i;
// 			}
// 		}
				
// 	}
// 	else
// 	{
// 		printf("Stripes full: No!\n");
// 		return -1;
// 	}
// }




// int Is_stripe_free(int n)
// {
// 	//printf("Is a stripe free\n");
// 	if (stripe_tbl[n].used == 0)
// 	{
// 		//printf("Yes!Free\n");
// 		return 1;
// 	}
// 	else
// 		return 0;
// }


void Init_a_stripe(struct stripe &s, int k, int m)
{
	// printf("Init a Stripe\n");
	s.stripe_num = stripe_num;
	s.data.resize(k, -1);
	s.parity.resize(m, -1);
	s.used = 1;
	stripe_tbl.push_back(s);
	used_stripe_num++;
	stripe_num++;

}

// /*return new stripe_num*/
long long Placement(struct SOE soe, int k, int m, int policy)
{
	struct stripe s;

	Init_a_stripe(s, k, m);

	assign_set.clear();

	for (int i = 0; i < k + m; ++i)
	{
		assign_set.push_back(Allocate_a_pblk());
		// printf("%lld ", assign_set[i]);
	}
	
	// printf("   XXXX\n");

	if(policy == 0)
	{
		Baseline_RR(s.stripe_num, 1);
	}
	else if(policy == 1)
	{
		Random(s.stripe_num);
	}
	else if(policy == 2)
	{
		Dual_Aware(s.stripe_num, M);
	}
	else
	{
		printf("Error!Placement Policy has to be [0|1|2]\n");
	}


	// for (int i = 0; i < K; ++i)
	// {
	// 	printf("soe.data_pblk[%d].p->fp->p->pblk_num = %d\n", i, soe.data_pblk[i].p->fp->p->pblk_num);
	// }

	for (int i = 0; i < k; ++i)
	{
		// printf("4\n");
		if (i > soe.data_pblk.size() - 1)
		{
			stripe_tbl[s.stripe_num].data[i] = -1;
		}
		else
		{
			stripe_tbl[s.stripe_num].data[i] = soe.data_pblk[i].p->fp->p->pblk_num;
			
			global_storage[soe.data_pblk[i].p->fp->p->pblk_num]->stripe_num = s.stripe_num;

			
		}
		
		// printf("stripe_tbl[%lld].data[%d] = %lld\n", s.stripe_num, i, stripe_tbl[s.stripe_num].data[i]);	
			
	}


	// printf("Start encoding a stripe\n");
	Encode_a_stripe(s.stripe_num, m);
	//Set_stripe_num(&stripe_tbl[stripe_num], stripe_num);

	return s.stripe_num;
}


void Baseline_RR(long long sp, int stride)
{
	for (int i = 0; i < sp * stride; ++i)
	{
		int j = assign_set.back();
		assign_set.pop_back();
		assign_set.push_front(j);
	}

	std::deque<long long> temp(0);

	for (int i = 0; i < K; ++i)
	{
		int ii = assign_set.front();
		assign_set.pop_front();
		temp.push_back(ii);
	}

	for (int i = 0; i < sp; ++i)
	{
		int j = temp.front();
		temp.pop_front();
		temp.push_back(j);
	}


	for (int i = 0; i < K; ++i)
	{
		if(i > soe.data_pblk.size()-1)
		{
			;
		}
		else
		{
			soe.data_pblk[i].p->fp->p = global_storage[temp[i]];
			global_storage[temp[i]]->fp = soe.data_pblk[i].p->fp;
		}
		
	}

	// for (int i = 0; i < K; ++i)
	// {
	// 	assign_set.pop_front();
	// }


	// // for (int i = 0; i < 3 + 2; ++i)
	// // {
	// // 	printf(" %lld\t", assign_set.at(i));
	// // }

	// printf("\n");
}


void Random(long long sp)
{
	
	//real random

	// printf("time(NULL) %lld\n", (long long)time(NULL) + sp);
	srand(time(NULL) + sp);

	// std::set<int> index(0);

	for (int i = 0; i < K; ++i)
	{
		
		int temp = rand()% (K + M - i);
		// while(index.find())
		// int temp = rand()% (K + M);
		// index.insert(temp);

		// printf("Temp[%d]: assign_set[temp] = %lld\n", temp, assign_set[temp]%8);
		
		if(i > soe.data_pblk.size()-1)
		{
			;
		}
		else
		{
			soe.data_pblk[i].p->fp->p = global_storage[assign_set[temp]];
			global_storage[assign_set[temp]]->fp = soe.data_pblk[i].p->fp;
			// printf("soe.data_pblk[%d].p->fp->p->pblk_num = %u\n", i, soe.data_pblk[i].p->fp->p->pblk_num);
			// printf("soe.data_pblk[%d].p->fp->p->pblk_num = %u\n", 0, soe.data_pblk[0].p->fp->p->pblk_num);
		}
		
		
		assign_set.erase(assign_set.begin()+temp);
		
	}


	// std::deque<long long>::iterator di;


	// for (int i = 0; i < soe.data_pblk.size()-1; ++i)
	// {
	// 	di = std::find(assign_set.begin(), assign_set.end(), soe.data_pblk[i].p->fp->p->pblk_num);
	// 	if(di !=  assign_set.end())
	// 	{
	// 		assign_set.erase(di);
	// 	}
		
	// }

	// for (int i = 0; i < K; ++i)
	// {
	// 	// printf("soe.data_pblk[%d].p->fp->p->pblk_num = %d\n", i, soe.data_pblk[i].p->fp->p->pblk_num);
	// }	

	//pseudo random 
	// std::random_shuffle(assign_set.begin(), assign_set.end());
	
	// for (int i = 0; i < K; ++i)
	// {
	// 	soe.data_pblk[i].p->pblk_nr = assign_set[i];
	// }

	// assign_set.erase(assign_set.begin(), assign_set.begin() + K);	
}

void Dual_Aware(long long sp, int stride)
{
	std::deque<long long> tmp(0);

	std::set<long long>::iterator si;

	for (int i = 0; i < sp * stride; ++i)
	{
		int j = assign_set.back();
		assign_set.pop_back();
		assign_set.push_front(j);
	}

	for (int i = 0; i < K; ++i)
	{
		long long t = assign_set.front();
		tmp.push_back(t);
		assign_set.pop_front();
		// printf("tmp[i]: %lld\n", tmp[i]);
	}

	//assignment
	for (int i = 0; i < K; ++i)
	{
		
		
		if(i > soe.data_pblk.size()-1)
		{
			// printf("i >>>>>> %d\n", i);;
		}
		else
		{
			// printf("i = %d\n", i);
			//initial value
			// soe.data_pblk[i].p->fp->p = global_storage[tmp[i]];
			// printf("i = %d\n", i);
			// global_storage[tmp[i]]->fp = soe.data_pblk[i].p->fp;

			// si = Req_Tbl[soe.data_pblk[i].Request_index].used_nodes.find(tmp[i] % N_num);

			// if (/* condition */)
			// {
			// 			/* code */
			// }		

			// printf("i= %d  ", i);

			// Print_Req_used_nodes(soe.data_pblk[i].Request_index);

			// printf("\n");

			for (int j = 0; j < tmp.size(); ++j)
			{


				si = Req_Tbl[soe.data_pblk[i].Request_index].used_nodes.find(tmp[j] % N_num);
				
				//Not find tmp[j]

				if(si == Req_Tbl[soe.data_pblk[i].Request_index].used_nodes.end())
				{
					// printf("soe.data_pblk[%d].p->fp->p->pblk_num = %lld\n", i, tmp[j]);
					soe.data_pblk[i].p->fp->p = global_storage[tmp[j]];
					global_storage[tmp[j]]->fp = soe.data_pblk[i].p->fp;

					Req_Tbl[soe.data_pblk[i].Request_index].used_nodes.insert(soe.data_pblk[i].p->fp->p->node_num);

					if(Req_Tbl[soe.data_pblk[i].Request_index].used_nodes.size() == K)
						Req_Tbl[soe.data_pblk[i].Request_index].used_nodes.clear();

					for (int m = 0; m < soe.data_pblk[i].dup.size(); ++m)
					{
						Req_Tbl[soe.data_pblk[i].dup[m]].used_nodes.insert(soe.data_pblk[i].p->fp->p->node_num);

						if(Req_Tbl[soe.data_pblk[i].dup[m]].used_nodes.size() == K)
							Req_Tbl[soe.data_pblk[i].dup[m]].used_nodes.clear();
					}


					tmp.erase(tmp.begin() + j);
					
					break;
				}
			}

			// printf("soe.data_pblk[%d].p->fp->p->pblk_num = %u\n", i, soe.data_pblk[i].p->fp->p->pblk_num);
			// printf("soe.data_pblk[%d].p->fp->p->pblk_num = %u\n", 0, soe.data_pblk[0].p->fp->p->pblk_num);
		}
		
	}



}

void Print_Cluster_Time()
{
	printf("Last_busy_time of each node\n");
	for (int i = 0; i < K+M; ++i)
	{
		printf("Cluster[%d]:%lld\n", i, Cluster[i]->Last_Busy_Time);
	}

	printf("\n");
}

int Search_fp_in_SOE(struct traceline *T_line, struct fp_node *s)
{
	for (int i = 0; i < soe.data_pblk.size(); ++i)
	{
		if(soe.data_pblk[i].Request_index == s->Request_index)
		{
			soe.data_pblk[i].dup.push_back(T_line->Request_index);
			return i;
		}
	}
}






// /*produce the parity and */
void Encode_a_stripe(int s, int m)
{
	for (int i = 0; i < m; ++i)
	{
		stripe_tbl[s].parity[i] = assign_set[i];
		global_storage[i]->parity = 1;
		global_storage[stripe_tbl[s].parity[i]]->stripe_num = s;
	}
}

// void Diff_EC(int group_num, int base_k, int base_m)
// {
// 	std::vector < LVec > m;
// 	m.resize(group_num);

// 	std::vector< long > div;
// 	div.resize(group_num-1);

// 	int d; 
// 	// d = Threshold(max_pblk_num);
	
// 	d = 2;

// 	for(int i = 0; i < used_pblk.size(); i++)
// 	{
// 		if(storage[used_pblk[i]]->ref_count >= d)
// 			m[0].push_back(used_pblk[i]);
// 		else
// 			m[1].push_back(used_pblk[i]);
// 	}	

// 	for(int j = 0; j < group_num; j++)
// 	{
// 		if(j == 0)
// 			//Print_vec(m[j]);

// 		printf("m[%d]: %lu\n", j, m[j].size());
		
// 		//printf("\nBatch[%d]: Encoding\n", j);
// 		Encode(&m[j], base_k + (d/2)*j, base_m - j*d/2);
// 	}

// }

// void Print_vec(LVec &p)
// {
// 	for(int j = 0; j < p.size(); j++)
// 	{
// 		if(j % 20 == 0)
// 			printf("\n");

// 		printf(" %ld", p[j]);		
// 	}
// 	printf("\n");
// }


// void Encode(LVec *p, int k, int m)
// {
// 	int counter = 0;
// 	int num = p->size() / k;
	
// 	Init_SOE(k);

// 	// printf("0 \n");
// 	for(int i = 0; i < p->size(); i++)
// 	{
// 		//printf("Starting iterating\n");
// 		Push_SOE(p->at(i));
		
// 		if(Is_SOE_full(k))
// 		{
// 			//printf("Full!\n");
// 			counter++;
// 			long sp = Placement(soe, k, m);
// 			//printf("Finish placement!\n");
// 			//Print_stripe(sp, k, m);
// 			Reset_SOE(k);
// 		}
// 		else if( (counter == num) && (i == p->size()-1) )
// 		{
// 			//printf("1\n");
// 			while(!Is_SOE_full(k))
// 				Push_SOE(-1);
// 			//printf("2\n");
// 			long sp = Placement(soe, k, m);
// 			Print_stripe(sp, k, m);
// 			Reset_SOE(k);
// 		}
			
// 	}

// }


// long Assign_parity()
// {
// 	if(pblk_used < MAX_BLK_NUM -1)
// 	{
// 		for (int i = pblk_serial; i < MAX_BLK_NUM; ++i)
// 		{
// 			if(Pblk_is_free(i))
// 			{
// 				storage[i]->ref_count = 1;
// 				storage[i]->node_num = i % N_num;
// 				storage[i]->parity = 1;

// 				storage[i]->fp = NULL;
// 				storage[i]->in_mem = 0; /*whether it is in the cache*/
// 				storage[i]->stripe_num = -1;
// 				storage[i]->corrupted = 0;
// 				storage[i]->pblk_num = i;

// 				pblk_used++;
// 				pblk_serial++;
				
// 				return i;
// 			}
// 		}
				
// 	}
// 	else
// 	{
// 		printf("Storage full: No space!\n");
// 		return -1;

// 	}
// }


void Init_SOE(int k)
{
	soe.data_pblk.resize(0);
	soe.full = 0;
}


void Push_SOE(struct traceline p)
{
	soe.data_pblk.push_back(p);
	soe.full++;
	//Print_SOE();
}

int Is_SOE_full(int k)
{
	return (soe.full == k);
}

void Reset_SOE(int k)
{
	
	soe.data_pblk.clear();
	soe.full = 0;
	// printf("Finish resetting SOE\n");
}

void Print_SOE()
{
	printf("Inside SOE is:\n");
	
	for (int i = 0; i < soe.full; ++i)
	{
		Print_traceline(&soe.data_pblk[i]);
	}
	printf("\n");
}

void Print_stripe(long stripe_num)
{
	printf("Stripe[%4ld]:  ", stripe_num);

	printf("Data[");

	for (int i = 0; i < stripe_tbl[stripe_num].data.size(); ++i)
	{
		if(stripe_tbl[stripe_num].data[i] == -1)
			printf(" %5lld", stripe_tbl[stripe_num].data[i]);
		else
			printf(" %5d", global_storage[stripe_tbl[stripe_num].data[i]]->node_num);
	}
	printf("]  ");

	printf("Parity[");

	for (int i = 0; i < stripe_tbl[stripe_num].parity.size(); ++i)
	{
		// printf(" %5lld", stripe_tbl[stripe_num].parity[i]);
		printf(" %4d", global_storage[stripe_tbl[stripe_num].parity[i]]->node_num);
	}
	printf("]");

	printf("\n");
}

// void Create_ref_statistics(std::map<long, long> &ref_statis)
// {
// 	for(int i = 0; i < used_pblk.size(); i++)
// 	{
// 		if(storage[used_pblk[i]]->ref_count > 0)
// 		{
// 			if(ref_statis.find(storage[used_pblk[i]]->ref_count) == ref_statis.end())
// 				ref_statis[storage[used_pblk[i]]->ref_count] = 1;
// 			else
// 				ref_statis[storage[used_pblk[i]]->ref_count]++;
// 		}
// 	}	

// 	std::map<long, long>::iterator mit;
// 	for(mit = ref_statis.begin(); mit != ref_statis.end(); mit++)
// 	{
// 		printf("[%ld  %ld] ", mit->first, mit->second);
// 	}

// 	printf("\n");
// }

// int Threshold(int max_pblk_num)
// {
// 	//std::vector<int> i;
// 	IVec *p;
// 	std::map <int, IVec *> ref_tbl;
// 	std::map <int, IVec *>::iterator mi;

// 	for(int i = 0; i < max_pblk_num; i++)
// 	{
// 		if(storage[i]->ref_count >= 1)
// 		{
// 			if(ref_tbl.find(storage[i]->ref_count) == ref_tbl.end())	
// 			{				
// 				ref_tbl[storage[pblk_serial]->ref_count] = (IVec *)malloc(sizeof(IVec));
// 			}
			
// 			printf("pblk_nr: %d ref: %d\n", i, storage[i]->ref_count);
// 			ref_tbl[storage[pblk_serial]->ref_count]->push_back(i);
// 		}
// 	}

// 	for(mi = ref_tbl.begin(); mi != ref_tbl.end(); mi++)
// 	{
// 		printf(" %d: %lu\n", mi->first, mi->second->size());
// 		//free(mi->second);
// 	}

// }

void Init(int node_num, int cache_size_N)
{
	Init_Cluster(node_num);
    Init_CurRequest();
    Init_LastReqestID();
    Set_cache_size(N_CACHE, cache_size_N);
}

void Destroy()
{
	Destroy_Cluster();
	Destroy_storage();
}