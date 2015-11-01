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
#include "stm_tinystm.h"



# define AL_LOCK(b)
# define PRINT_STATS()
# define SETUP_NUMBER_TASKS(b)
# define SETUP_NUMBER_THREADS(b)

# define NULL_O_SET   1
#define FIRST_O_SET   1
#define FETCH_INC_CLOCK()                 ({stm_inc_clock();  \
                                            stm_get_clock(); })

#ifdef REDUCED_TM_API
#    define tinystm_Self                /*nothing*/
#    define TM_ARG_ALONE                /*nothing*/
#    define SPECIAL_THREAD_ID()         get_tid()
#    define SPECIAL_INIT_THREAD(id)     thread_desc[id] = (void*)TM_ARG_ALONE;
#    define TM_THREAD_ENTER()        stm_init_thread()
                                     STM_INIT_THREAD(inited_thread, SPECIAL_THREAD_ID()); \
                                     thread_desc[SPECIAL_THREAD_ID()] = (void*)inited_thread;
#else
//#    define TM_ARG_ALONE                  /*nothing*/
#    define TM_ARG_ALONE		            /*nothing*/
#    define SPECIAL_THREAD_ID()         thread_getId()
#    define TM_ARGDECL                   /*nothing */
//#    define TM_ARGDECL                    STM_THREAD_T* TM_ARG
#    define TM_ARGDECL_ALONE              struct stm_tx* TM_ARG_ALONE
#    define TM_THREAD_ENTER()         stm_init_thread()

#endif
#    define TM_CALLABLE                   /* nothing */
#    define TM_ARG                        /*nothing */
#    define TM_THREAD_EXIT()          stm_exit_thread()

#      include <mod_mem.h>
#      include <mod_stats.h>


#      define TM_STARTUP(numThread,useless)     if (sizeof(long) != sizeof(void *)) { \
                                          fprintf(stderr, "Error: unsupported long and pointer sizes\n"); \
                                          exit(1); \
                                        } \
                                        stm_init(); \
                                        mod_mem_init(0); \
                                        if (getenv("STM_STATS") != NULL) { \
                                          mod_stats_init(); \
                                        }

#      define TM_SHUTDOWN()             \
                                               if (getenv("STM_STATS") != NULL) { \
                                          unsigned long u; \
                                          if (stm_get_global_stats("global_nb_commits", &u) != 0) \
                                            printf("#commits    : %lu\n", u); \
                                          if (stm_get_global_stats("global_nb_aborts", &u) != 0) \
                                            printf("#aborts     : %lu\n", u); \
                                          if (stm_get_global_stats("global_max_retries", &u) != 0) \
                                            printf("Max retries : %lu\n", u); \
                                        } \
                                        stm_exit(); \
    unsigned long commits = 0; \
    unsigned long aborts = 0; \
    int ik = 0; \
    for (; ik < 128; ik++) { \
       if (statistics_array[ik].commits == 0) { break; } \
       commits += statistics_array[ik].commits; \
       aborts += statistics_array[ik].aborts; \
    } \
    printf("Total commits: %lu\nTotal aborts: %lu\n", commits, aborts); \

#  define TM_BEGIN_WAIVER()
#  define TM_END_WAIVER()

# define TM_BEGIN(b,mode)     TM_BEGIN_EXT(b,mode,0)
# define SPEND_BUDGET(b)	if(RETRY_POLICY == 0) (*b)=0; else if (RETRY_POLICY == 2) (*b)=(*b)/2; else (*b)=--(*b);


#    define TM_BEGIN_EXT(b,mode,ro) { \
    tries = HTM_RETRIES;\
		while (1) {   \
  		if (tries > 0) { \
                      TM_buff_type TM_buff; \
                      unsigned int status = __TM_begin(&TM_buff); \
                      if (status == _HTM_TBEGIN_STARTED) { \
  			                  o_set_pointer = init_o_set();   \
                          break;  \
                      }   \
  		                else{ \
                      	tries--;    \
  		                } \
       } else  {   \
  			mode = 1; \
        sigjmp_buf buf; \
        sigsetjmp(buf, 0); \
        stm_tx_attr_t _a = {}; \
        _a.read_only = ro; \
        stm_start(_a, &buf); \
        sigsetjmp(buf, 0); \
        statistics_array[SPECIAL_THREAD_ID()].aborts++; \
        }   \
    }   \
  


#    define TM_END()    \
        stm_word_t commit_timestamp;   \
        stm_word_t *orec;   \
        if ((tries > 0) ) {    \
            if(o_set_pointer->first_o_set != NULL_O_SET)  {   \
                __TM_suspend();   \
                commit_timestamp = FETCH_INC_CLOCK(); \ 
                __TM_resume();  \
                while (o_set_pointer->first_o_set != FIRST_O_SET){ \
                    stm_word_t *orec = GET_LOCK_ALIAS(o_set_pointer->address);  \
                    atomic_store_rel(orec,commit_timestamp); \
                    o_set_pointer=(o_set_pointer->previous_o_set); \
                }    \
            }    \
         __TM_end(); \
        } else {    \
           stm_commit(); \
           statistics_array[SPECIAL_THREAD_ID()].aborts--; \
           statistics_array[SPECIAL_THREAD_ID()].commits++;  \
        } \
        statistics_array[SPECIAL_THREAD_ID()].commits++; \
};
  

#      define P_MALLOC(size)            malloc(size)
#      define P_FREE(ptr)               free(ptr)
#      define SEQ_MALLOC(size)          malloc(size)
#      define SEQ_FREE(ptr)             free(ptr)

#      define TM_MALLOC(size)           malloc(size)
#      define FAST_PATH_FREE(ptr)        free(ptr)
#      define SLOW_PATH_FREE(ptr)

# define GET_LOCK_ALIAS(var)    get_lock((void*)&var);
# define NULL 0
# define IS_LOCKED(l)         is_locked(l)

# define FAST_PATH_RESTART() __TM_abort();


#  include <wrappers.h>


inline intptr_t hytm_TxLoad(intptr_t* var);
inline intptr_t hytm_TxLoad(intptr_t* var){   stm_word_t *orec = GET_LOCK_ALIAS(var);   \
                                              if (IS_LOCKED(orec))  \
                                                  FAST_PATH_RESTART(); \
                                              __TM_suspend(); \
                                             int temp = *var;
                                              __TM_resume();  \
                                              return temp;}

# define FAST_PATH_SHARED_READ(var) hytm_TxLoad((stm_word_t*)(void*)&(var))

# define FAST_PATH_SHARED_READ_P(var) hytm_TxLoad((stm_word_t*)(void*)&(var))

# define FAST_PATH_SHARED_READ_D(var) hytm_TxLoad((stm_word_t*)(void*)&(var))


# define FAST_PATH_SHARED_WRITE(var, val) ({  stm_word_t *orec = GET_LOCK_ALIAS(var); \   
                                                    if (IS_LOCKED(orec)) {  \
                                                         FAST_PATH_RESTART(); \
                                                        }   \    
                                                         var = val; \
                                                        o_set_pointer = add_to_o_set(o_set_pointer, var);}) 

# define FAST_PATH_SHARED_WRITE_P(var, val) ({  stm_word_t *orec = GET_LOCK_ALIAS(var); \   
                                                    if (IS_LOCKED(orec)) {  \
                                                         FAST_PATH_RESTART(); \
                                                        }   \    
                                                         var = val; \
                                                       o_set_pointer = add_to_o_set(o_set_pointer, var);}) 

# define FAST_PATH_SHARED_WRITE_D(var, val) ({  stm_word_t *orec = GET_LOCK_ALIAS(var); \   
                                                    if (IS_LOCKED(orec)) {  \
                                                         FAST_PATH_RESTART(); \
                                                        }   \    
                                                         var = val; \
                                                        o_set_pointer = add_to_o_set(o_set_pointer, var);})    


# define SLOW_PATH_RESTART() STM_RESTART();
# define SLOW_PATH_SHARED_READ(var)           stm_load((volatile stm_word_t *)(void *)&(var))
# define SLOW_PATH_SHARED_READ_P(var)         stm_load_ptr((volatile void **)(void *)&(var))
# define SLOW_PATH_SHARED_READ_F(var)         stm_load_float((volatile float *)(void *)&(var))
# define SLOW_PATH_SHARED_READ_D(var)         stm_load_double((volatile double *)(void *)&(var))
# define SLOW_PATH_SHARED_WRITE(var, val)     stm_store((void*)&(var), val)
# define SLOW_PATH_SHARED_WRITE_P(var, val)   stm_store((void*)&(var), val)
# define SLOW_PATH_SHARED_WRITE_D(var, val)   stm_store((void*)&(var), val)

#  define TM_LOCAL_WRITE(var, val)      ({var = val; var;})
#  define TM_LOCAL_WRITE_P(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_D(var, val)    ({var = val; var;})

#endif /* TM_H */
