#include "cache.h"



int Set_cache_size(int cache_num, long long size)
{
	if (size <= 0)
	{
		N_cache_size = -1;
	}
	else
	{
		if(cache_num == 0)
		{		 
			 N_cache_size = size;
		}
		else if(cache_num == 1)
		{
			
			F_cache_size = size;
		}
		else if(cache_num == 2)
		{
			N_cache_size = size;
			F_cache_size = size;
		}
	}
	
}

int Insert_cache(int cache_num, struct page_node *p)
{

	std::list< struct fp_node *> *cache = NULL; 
	if(cache_num == 0)
	{
		cache = &N_cache;
		Mark_fp_in_N_cache(p->fp);
	}
	else if(cache_num == 1)
	{
		cache = &F_cache;
		Mark_fp_in_F_cache(p->fp);
	}

	cache->push_front(p->fp);

	return 0;
}

void Evict_cache(int cache_num)
{

	std::list< struct fp_node *> *cache = NULL; 
	std::list< struct fp_node *>::iterator li;

	struct fp_node *p = NULL;

	
	if(cache_num == 0)
	{
		cache = &N_cache;
		p = cache->back();
		Mark_fp_not_in_N_cache(p);
	}
	else if(cache_num == 1)
	{
		cache = &F_cache;
		p = cache->back();
		Mark_fp_not_in_F_cache(p);
	}

	return cache->pop_back();
}

int Hit_cache(int cache_num, struct page_node * p)
{
	int N_hit = 0;
	int F_hit = 0;

	// printf("inside Hit_cache: p->pblk_nr: %u\n", p->pblk_nr);
	N_hit = fp_is_in_N_cache(p->fp);
	// printf("inside Hit_cache\n");

	F_hit = fp_is_in_F_cache(p->fp);
	
	if(cache_num == 0)
	{		 
		 return N_hit;
	}
	else if(cache_num == 1)
	{
		
		return F_hit;
	}
	else if(cache_num == 2)
	{
		return F_hit || N_hit;
	}

}

int LRU_Ajust(std::list< struct fp_node *> &cache, struct page_node * p)
{

	std::list< struct fp_node *>::iterator findIter = std::find(cache.begin(), cache.end(), p->fp);

	if(findIter == cache.end())
	{
		printf("Not in the cache!\n");
	}
	else
	{
		cache.erase(findIter);	
		cache.push_front(p->fp);
	}

}


int Cache_full(int cache_num)
{
	if(cache_num == 0)
	{		 
		 return (N_cache.size() >= N_cache_size);
	}
	else if(cache_num == 1)
	{
		
		return (F_cache.size() >= F_cache_size);
	}
	else if(cache_num == 2)
	{
		return -1;
		//return F_hit || N_hit;
	}

}

void Print_cache(std::list< struct fp_node *> &cache)
{
	std::list< struct fp_node *>::iterator li;
	
	for (li = cache.begin(); li != cache.end(); ++li)
	{
		printf("%s \n", (*li)->fingerprint);
		// Print_pblk((*li)->pblk_num);
	}
	printf("\n");
}


int Routine_N_cache(struct page_node * p)
{
	if (N_cache_size <= 0)
	{
		printf("No cache! [Miss]\n");
		return 0;
	}
	else
	{
		if(Hit_cache(N_CACHE, p) == 1)
		{
			printf("line[%lld]: cache [Hit]\n", total_line);
			// printf("Before LRU_Ajust\n");
			LRU_Ajust(N_cache, p);
			return 1;
		}
		else//cache miss
		{
			printf("line[%lld]: cache [Miss]\n", total_line);
			if(Cache_full(N_CACHE))//cache full
			{
				printf("N_cache is full![Eviciting]\n");
				Evict_cache(N_CACHE);
				Insert_cache(N_CACHE, p);
			}
			else//cache not full
			{
				// printf("[not Full] Insert a new page\n");
				Insert_cache(N_CACHE, p);
			}

			return 0;
		}

	}
	
}

void Clean_cache(int cache_num)
{
	if(cache_num == 0)
	{		 
		N_cache.clear();
		// N_cache.swap(std::list< struct fp_node *>(N_cache));
	}
	else if(cache_num == 1)
	{
		
		F_cache.clear();
	}
	else if(cache_num == 2)
	{
			 
		N_cache.clear();
		F_cache.clear();
		//return F_hit || N_hit;
	}

}