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

#  include <string.h>
#  include <stm_tl2.h>
#  include "tl2.h"
#ifndef REDUCED_TM_API
#  include "thread.h"
#endif

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


# define SETUP_NUMBER_TASKS(p);
# define SETUP_NUMBER_THREADS(p);
# define PRINT_STATS()

#      define TM_STARTUP(numThread,useless)     STM_STARTUP()
#  define TM_SHUTDOWN() { \
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


#      define P_MALLOC(size)            malloc(size)
#      define P_FREE(ptr)               free(ptr)
#      define TM_MALLOC(size)          malloc(size)
#      define FAST_PATH_FREE(ptr)		
#      define SLOW_PATH_FREE(ptr)		

# define TM_BEGIN(b,mode)     TM_BEGIN_EXT(b,mode,0)
#    define TM_BEGIN_EXT(b,m,ro)     { \
    STM_BEGIN(ro); \
    statistics_array[SPECIAL_THREAD_ID()].aborts++; \

#    define TM_END()   \
    STM_END(); \
    statistics_array[SPECIAL_THREAD_ID()].aborts--; \
    statistics_array[SPECIAL_THREAD_ID()].commits++; \
}

#    define FAST_PATH_RESTART()          STM_RESTART()
#    define SLOW_PATH_RESTART()          FAST_PATH_RESTART()

#    define TM_EARLY_RELEASE(var)       /* nothing */

#  define FAST_PATH_SHARED_READ(var)     STM_READ(var)
#  define FAST_PATH_SHARED_READ_P(var)   STM_READ_P(var)
#  define FAST_PATH_SHARED_READ_D(var)   STM_READ_D(var)
#  define FAST_PATH_SHARED_READ_F(var)   STM_READ(var)
#  define FAST_PATH_SHARED_WRITE(var, val)     STM_WRITE((var), val)
#  define FAST_PATH_SHARED_WRITE_P(var, val)   STM_WRITE_P((var), val)
#  define FAST_PATH_SHARED_WRITE_D(var, val)   STM_WRITE_D((var), val)

# define SLOW_PATH_SHARED_READ(var)           FAST_PATH_SHARED_READ(var)
# define SLOW_PATH_SHARED_READ_P(var)         FAST_PATH_SHARED_READ_P(var)
# define SLOW_PATH_SHARED_READ_F(var)         FAST_PATH_SHARED_READ_F(val)
# define SLOW_PATH_SHARED_READ_D(var)         FAST_PATH_SHARED_READ_D(var)
# define SLOW_PATH_SHARED_WRITE(var, val)     FAST_PATH_SHARED_WRITE(var, val)
# define SLOW_PATH_SHARED_WRITE_P(var, val)   FAST_PATH_SHARED_WRITE_P(var, val)
# define SLOW_PATH_SHARED_WRITE_D(var, val)   FAST_PATH_SHARED_WRITE_D(var, val)

#  define TM_LOCAL_WRITE(var, val)      STM_LOCAL_WRITE(var, val)
#  define TM_LOCAL_WRITE_P(var, val)    STM_LOCAL_WRITE_P(var, val)
#  define TM_LOCAL_WRITE_D(var, val)    STM_LOCAL_WRITE_D(var, val)

#endif /* TM_H */
