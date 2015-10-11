#ifndef TM_H
#define TM_H 1

#  include <stdio.h>

#  define MAIN(argc, argv)              int main (int argc, char** argv)
#  define MAIN_RETURN(val)              return val

#  define GOTO_SIM()                    /* nothing */
#  define GOTO_REAL()                   /* nothing */
#  define IS_IN_SIM()                   (0)

#  define SIM_GET_NUM_CPU(var)          /* nothing */

#  define TM_PRINTF                     printf
#  define TM_PRINT0                     printf
#  define TM_PRINT1                     printf
#  define TM_PRINT2                     printf
#  define TM_PRINT3                     printf

#  define P_MEMORY_STARTUP(numThread)   /* nothing */
#  define P_MEMORY_SHUTDOWN()           /* nothing */

#  include <assert.h>
#ifndef REDUCED_TM_API
#  include "memory.h"
#  include "thread.h"
#  include "types.h"
#endif

//#include <immintrin.h>
//#include <rtmintrin.h>
#include <htmxlintrin.h>
//#include "stm_internal.h"
//#include "tl2.h"
#include "stm_tinystm.h"

# define AL_LOCK(b)
# define PRINT_STATS()
# define SETUP_NUMBER_TASKS(b)
# define SETUP_NUMBER_THREADS(b)

#ifdef REDUCED_TM_API
#    define tinystm_Self                TM_ARG_ALONE
#    define TM_ARG_ALONE                &stm_current_tx()
#    define SPECIAL_THREAD_ID()         get_tid()
#    define SPECIAL_INIT_THREAD(id)     thread_desc[id] = (void*)TM_ARG_ALONE;
#    define TM_THREAD_ENTER()         Thread* inited_thread = STM_NEW_THREAD(); \
                                     STM_INIT_THREAD(inited_thread, SPECIAL_THREAD_ID()); \
                                     thread_desc[SPECIAL_THREAD_ID()] = (void*)inited_thread;
#else
//#    define TM_ARG_ALONE                  tinystm_Self
#    define TM_ARG_ALONE		          &stm_current_tx()
#    define SPECIAL_THREAD_ID()         thread_getId()
#    define TM_ARGDECL                   TINYSTM_TX TM_ARG
//#    define TM_ARGDECL                    STM_THREAD_T* TM_ARG
#    define TM_ARGDECL_ALONE              stm_tx* TM_ARG_ALONE
#    define TM_THREAD_ENTER()         TM_ARGDECL_ALONE = STM_NEW_THREAD(); \
                                      STM_INIT_THREAD(TM_ARG_ALONE, SPECIAL_THREAD_ID());
#endif
#    define TM_CALLABLE                   /* nothing */
#    define TM_ARG                        TM_ARG_ALONE,
#    define TM_THREAD_EXIT()          STM_FREE_THREAD(TM_ARG_ALONE)

#      define TM_STARTUP(numThread, useless)     STM_STARTUP()
#      define TM_SHUTDOWN() { \
    STM_SHUTDOWN(); \
    unsigned long commits = 0; \
    unsigned long aborts = 0; \
    int i = 0; \
    for (; i < 128; i++) { \
       if (statistics_array[i].commits == 0) { break; } \
       commits += statistics_array[i].commits; \
       aborts += statistics_array[i].aborts; \
    } \
    printf("Total commits: %lu\nTotal aborts: %lu\n", commits, aborts); \
}

#  define TM_BEGIN_WAIVER()
#  define TM_END_WAIVER()

# define TM_BEGIN(b,mode)     TM_BEGIN_EXT(b,mode,0)
# define SPEND_BUDGET(b)	if(RETRY_POLICY == 0) (*b)=0; else if (RETRY_POLICY == 2) (*b)=(*b)/2; else (*b)=--(*b);

#    define TM_BEGIN_EXT(b,mode,ro) { \
        unsigned int counter_stm_executions = 0; \
		int tries = HTM_RETRIES;\
        o_set* o_set_atribute; \
        orec* orecs;  \
		while (1) {   \
			if (tries > 0) { \
                if (mode == 0)  {   \
                    TM_buff_type TM_buff; \
                    unsigned int status = __TM_begin(&TM_buff); \
                    if (status == _HTM_TBEGIN_STARTED) { \
                        break;  \
                    }   \
                    /*initialize the o-set*/   \
                    tries--;    \
                }       \
                if(mode == 1)   {   \
                    stm_init(); \
                }   \
            }   \
	}


#    define TM_END()    \
        int commit_timestamp = 0;   \
        o_set* o_set_pointer = o_set_atribute; \; \
        orec* fetched_orec;   \
        if ((tries > 0) ) {    \
            if((o_set_atribute->previous_o_set != 0) && (o_set_atribute->next_o_set != 0))  {   \
                commit_timestamp = __sync_add_and_fetch(&htm_clock.counter,1); \
                while ( (o_set_pointer->previous_o_set != 0) ){ \
                    fetched_orec = fetch_orec(orecs,(o_set_pointer->address));  \
                    fetched_orec->locked=0; \
                    o_set_pointer = (o_set_pointer->previous_o_set);    \
                }    \
                fetched_orec = fetch_orec (orecs,(o_set_pointer->address)); \
                fetched_orec->locked=0; \
            }    \
         __TM_end(); \
        } else {    \
           commit_timestamp =  __sync_add_and_fetch(&htm_clock.counter,1);    \
            int ret = HYBRID_STM_END();  \
            commit_timestamp = __sync_sub_and_fetch(&htm_clock.counter,1);    \
            if (ret == 0) { \
                STM_RESTART(); \
            } \
            __sync_sub_and_fetch(&htm_clock.counter,1); \
            statistics_array[SPECIAL_THREAD_ID()].aborts--; \
        } \
        statistics_array[SPECIAL_THREAD_ID()].commits++; \
};
      

# define FETCH_OREC (addr)            ({     orec* ptr = orecs; \
                                             ptr = ptr + addr;  \
                                             ptr; }  )

#      define MALLOC_OREC_STRUCT(ptr)       ({ptr = malloc(sizeof(orec));})


#      define P_MALLOC(size)            malloc(size)
#      define P_FREE(ptr)               free(ptr)
#      define SEQ_MALLOC(size)          malloc(size)
#      define SEQ_FREE(ptr)             free(ptr)

#      define TM_MALLOC(size)           malloc(size)
#      define FAST_PATH_FREE(ptr)        free(ptr)
#      define SLOW_PATH_FREE(ptr)

# define FAST_PATH_RESTART() __TM_abort();
# define FAST_PATH_SHARED_READ(var) ({  orec* orec = fetch_orec(var,orecs);    \
                                          if ((orec->locked) == 1)  \
                                             { __TM_abort(); }  \
                                               var;})



# define FAST_PATH_SHARED_READ_P(var) ({   orec* orec = fetch_orec(var,orecs);    \
                                          if ((orec->locked) == 1)  \
                                             { __TM_abort(); }  \
                                               var;})

# define FAST_PATH_SHARED_READ_D(var) ({  orec* orec = fetch_orec(var,orecs);    \
                                          if ((orec->locked) == 1)  \
                                             { __TM_abort(); }  \
                                               var;})


# define FAST_PATH_SHARED_WRITE(var, val) ({  orec* orec = fetch_orec(var,orecs); \   
                                                    if ((orec->locked) == 1) {  \
                                                         __TM_abort(); \
                                                        }   \    
                                                        var;    \
                                                         var = val; \
                                                        o_set_atribute = add_to_o_set(o_set_atribute, var);}) 

# define FAST_PATH_SHARED_WRITE_P(var, val) ({  orec* orec = fetch_orec(var,orecs); \   
                                                    if ((orec->locked) == 1) {  \
                                                         __TM_abort(); \
                                                        }   \    
                                                        var;    \
                                                         var = val; \
                                                        o_set_atribute = add_to_o_set(o_set_atribute, var);}) 

# define FAST_PATH_SHARED_WRITE_D(var, val) ({  orec* orec = fetch_orec(var,orecs); \   
                                                    if ((orec->locked) == 1) {  \
                                                         __TM_abort(); \
                                                        }   \    
                                                        var;    \
                                                         var = val; \
                                                        o_set_atribute = add_to_o_set(o_set_atribute, var);})    


# define SLOW_PATH_RESTART() STM_RESTART();
# define SLOW_PATH_SHARED_READ(var)           stm_load((void*)&(var))
# define SLOW_PATH_SHARED_READ_P(var)         stm_load((void*)&(var))
# define SLOW_PATH_SHARED_READ_F(var)         stm_load((void*)&(var))
# define SLOW_PATH_SHARED_READ_D(var)         stm_load((void*)&(var))
# define SLOW_PATH_SHARED_WRITE(var, val)     stm_store((void*)&(var), val)
# define SLOW_PATH_SHARED_WRITE_P(var, val)   stm_store((void*)&(var), val)
# define SLOW_PATH_SHARED_WRITE_D(var, val)   stm_store((void*)&(var), val)

#  define TM_LOCAL_WRITE(var, val)      ({var = val; var;})
#  define TM_LOCAL_WRITE_P(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_D(var, val)    ({var = val; var;})

#endif /* TM_H */
