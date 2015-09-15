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

#include <immintrin.h>
#include <rtmintrin.h>
#include "stm_tl2.h"
#include "tl2.h"

# define AL_LOCK(b)
# define PRINT_STATS()
# define SETUP_NUMBER_TASKS(b)
# define SETUP_NUMBER_THREADS(b)

#ifdef REDUCED_TM_API
#    define Self                        TM_ARG_ALONE
#    define TM_ARG_ALONE                get_thread()
#    define SPECIAL_THREAD_ID()         get_tid()
#    define SPECIAL_INIT_THREAD(id)     thread_desc[id] = (void*)TM_ARG_ALONE;
#    define TM_THREAD_ENTER()         Thread* inited_thread = STM_NEW_THREAD(); \
                                      STM_INIT_THREAD(inited_thread, SPECIAL_THREAD_ID()); \
                                      thread_desc[SPECIAL_THREAD_ID()] = (void*)inited_thread;
#else
#    define TM_ARG_ALONE                  STM_SELF
#    define SPECIAL_THREAD_ID()         thread_getId()
#    define TM_ARGDECL                    STM_THREAD_T* TM_ARG
#    define TM_ARGDECL_ALONE              STM_THREAD_T* TM_ARG_ALONE
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
		int tries = HTM_RETRIES;	\
		while (1) {	\
			if (tries > 0) { \
				while (fallback_in_use.counter != 0) { __asm__ ( "pause;" ); } \
				unsigned int status = _xbegin();	\
				if (status == _XBEGIN_STARTED) { \
					if (fallback_in_use.counter != 0) { _xabort(0xab); } \
                    if (ro == 0) { next_commit = NEXT_CLOCK(); } \
					break;	\
				} \
				else if (status == _XABORT_CAPACITY) { \
               SPEND_BUDGET(&tries); \
            } \
            else { \
               tries--; \
            } \
            statistics_array[SPECIAL_THREAD_ID()].aborts++; \
			} else {  \
				mode = 1; \
				STM_BEGIN(ro); \
				statistics_array[SPECIAL_THREAD_ID()].aborts++; \
				break;  \
			} \
	}


#    define TM_END()		\
		if (tries > 0) {	\
			_xend();	\
		} else {	\
			__sync_add_and_fetch(&fallback_in_use.counter,1);    \
			int ret = HYBRID_STM_END();  \
			__sync_sub_and_fetch(&fallback_in_use.counter,1);    \
			if (ret == 0) { \
				STM_RESTART(); \
			} \
			AFTER_COMMIT(); \
			statistics_array[SPECIAL_THREAD_ID()].aborts--; \
		} \
		statistics_array[SPECIAL_THREAD_ID()].commits++; \
};


#    define TM_EARLY_RELEASE(var)         

#      define P_MALLOC(size)            malloc(size)
#      define P_FREE(ptr)               free(ptr)
#      define SEQ_MALLOC(size)          malloc(size)
#      define SEQ_FREE(ptr)             free(ptr)

#      define TM_MALLOC(size)           malloc(size)
#      define FAST_PATH_FREE(ptr)        free(ptr)
#      define SLOW_PATH_FREE(ptr)

# define FAST_PATH_RESTART() _xabort(0xab);
# define FAST_PATH_SHARED_READ(var) (var)
# define FAST_PATH_SHARED_READ_P(var) (var)
# define FAST_PATH_SHARED_READ_D(var) (var)
# define FAST_PATH_SHARED_WRITE(var, val) ({HTM_WRITE((vintp*)(void*)&(var), (intptr_t)val, next_commit); var = val; var;})
# define FAST_PATH_SHARED_WRITE_P(var, val) ({HTM_WRITE((vintp*)(void*)&(var), VP2IP(val), next_commit); var = val; var;})
# define FAST_PATH_SHARED_WRITE_D(var, val) ({HTM_WRITE((vintp*)DP2IPP(&(var)), D2IP(val), next_commit); var = val; var;})

# define SLOW_PATH_RESTART() STM_RESTART();
# define SLOW_PATH_SHARED_READ(var)           STM_READ(var)
# define SLOW_PATH_SHARED_READ_P(var)         STM_READ_P(var)
# define SLOW_PATH_SHARED_READ_F(var)         STM_READ(var)
# define SLOW_PATH_SHARED_READ_D(var)         STM_READ_D(var)
# define SLOW_PATH_SHARED_WRITE(var, val)     STM_WRITE((var), val)
# define SLOW_PATH_SHARED_WRITE_P(var, val)   STM_WRITE_P((var), val)
# define SLOW_PATH_SHARED_WRITE_D(var, val)   STM_WRITE_D((var), val)

#  define TM_LOCAL_WRITE(var, val)      ({var = val; var;})
#  define TM_LOCAL_WRITE_P(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_D(var, val)    ({var = val; var;})

#endif /* TM_H */
