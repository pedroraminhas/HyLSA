/* =============================================================================
 *
 * stm.h
 *
 * User program interface for STM. For an STM to interface with STAMP, it needs
 * to have its own stm.h for which it redefines the macros appropriately.
 *
 * =============================================================================
 *
 * Author: Chi Cao Minh
 *
 * =============================================================================
 */


#ifndef STM_H
#define STM_H 1


#include "tl2.h"
#include "util.h"

//Read-Set structure
typedef struct read_set {
    unsigned long address;
    unsigned long val;
    unsigned long version;
} __attribute__((aligned(CACHE_LINE_SIZE))) read_set_t;

//Write-set structure
typedef struct write_set {
    unsigned long address;
    unsigned long val;
} __attribute__((aligned(CACHE_LINE_SIZE))) write_set_t;

extern __attribute__((aligned(CACHE_LINE_SIZE))) orec* orecs; //TO DO

#define STM_THREAD_T                    Thread
#define STM_SELF                        Self
#define STM_RO_FLAG                     ROFlag

#define STM_MALLOC(size)                TxAlloc(STM_SELF, size)
#define STM_FREE(ptr)                   TxFree(STM_SELF, ptr)




#  include <setjmp.h>
#  define STM_JMPBUF_T                  sigjmp_buf
#  define STM_JMPBUF                    buf

//Operations to the Orecs
# define FETCH_OREC (addr)            ({     orec* ptr = orecs;
                                             ptr = ptr + addr;
                                             ptr; }  )
//Operations to the Write-set

# define IS_IN_WRITE_SET(var) ({  write_set_t* ptr= write_set;
                                                write_set_t* endPtr = write_set + sizeof(write_set)/sizeof(write-set[0]);
                                                while ( ptr < endPtr ){
                                                      if(ptr.address == var) 
                                                        1;
                                                        ptr++;
                                                    } 
                                                    0;
                                                })
# define GET_VALUE_IN_WRITE_SET(var, val) ({  write_set_t* ptr = write-set;
                                                write_set_t* endPtr = write_set + sizeof(write_set)/sizeof(write-set[0]);
                                                while (ptr < endPtr) {
                                                    if (ptr.address == var)
                                                        ptr.val;
                                                  }
                                                })

//Validate operation
# define validate(read-set) ({read_set_t* read_set_beginPtr = read-set;
                              read_set_t* read_set_endPr = read_set + sizeof(read_set)/sizeof(read-set[0]);
                              while (read_set_beginPtr < read_set_endPr) {
                                addr = read_set_beginPtr.address;
                                version = read_set_beginPtr.ver;
                                orec* orec = FETCH_OREC(addr);
                                if ((orec.locked && (orec.owner != 0 /*TODO*/)) || (!orec.locked && (orec.version != version)))
                                    { _xabort(0xab); 0}
                                    read_set_beginPtr++;
                              }
            1;
    })

#define STM_VALID()                     (1)
#define STM_RESTART()                   TxAbort(STM_SELF)

#define STM_STARTUP()                   TxOnce()
#define STM_SHUTDOWN()                  TxShutdown()

#define STM_NEW_THREAD()                TxNewThread()
#define STM_INIT_THREAD(t, id)          TxInitThread(t, id)
#define STM_FREE_THREAD(t)              TxFreeThread(t)








#  define STM_BEGIN(isReadOnly)         do { \
                                            unsigned long clock = 0;
                                            unsigned long lowerBound = clock;
                                            unsigned long upperBound = clock;
                                            read_set_t read-set = null;
                                            write_set_t write-set = null;

                                            STM_JMPBUF_T STM_JMPBUF; \
                                            int STM_RO_FLAG = isReadOnly; \
                                            sigsetjmp(STM_JMPBUF, 0); \
                                            TxStart(STM_SELF, &STM_JMPBUF, &STM_RO_FLAG); \
                                        } while (0) /* enforce comma */

#define STM_BEGIN_RD()                  STM_BEGIN(1)
#define STM_BEGIN_WR()                  STM_BEGIN(0)
#define STM_END()                       TxCommit(STM_SELF)
#define HYBRID_HTM_END()				 TxCommitNoAbortHTM(STM_SELF)
#define HYBRID_STM_END()				 TxCommitNoAbortSTM(STM_SELF)
#define AFTER_COMMIT()					 AfterCommit(STM_SELF)
#define NEXT_CLOCK()					 GVGenerateWV(STM_SELF, 0)

typedef volatile intptr_t               vintp;

#define HYBRID_READ(var)		TxLoad(STM_SELF, var)
#define HYBRID_WRITE(var, val)		TxStore(STM_SELF, var, val)

#define STM_READ(var)                  ({ orec* fetched_orec = FETCH_OREC(var);
                                          unsigned long val = 0; 

                                          if(fetched_orec.locked){
                                            if(fetched_orec.owner != /*another thread*/)
                                                { _xabort(0xab);} //orec owned by other sw transaction
                                            if(IS_IN_WRITE_SET(var))
                                              val = GET_VALUE_IN_WRITE_SET(var);
                                            }else{
                                                if(fetched_orec.version > upperBound)
                                                    upperBound = clock;

                                            }

                                        }) TxLoad(STM_SELF, (vintp*)(void*)&(var))


#define STM_READ_D(var)                 IP2D(TxLoad(STM_SELF, \
                                                    (vintp*)DP2IPP(&(var))))
#define STM_READ_P(var)                 IP2VP(TxLoad(STM_SELF, \
                                                     (vintp*)(void*)&(var)))

#define STM_WRITE(var, val)             TxStore(STM_SELF, \
                                                (vintp*)(void*)&(var), \
                                                (intptr_t)(val))
#define STM_WRITE_D(var, val)           TxStore(STM_SELF, \
                                                (vintp*)DP2IPP(&(var)), \
                                                D2IP(val))
#define STM_WRITE_P(var, val)           TxStore(STM_SELF, \
                                                (vintp*)(void*)&(var), \
                                                VP2IP(val))

#define HTM_WRITE(var, val, clock)	     TxStoreHTM(STM_SELF, var, val, clock)

#define STM_LOCAL_WRITE(var, val)       ({var = val; var;})
#define STM_LOCAL_WRITE_D(var, val)     ({var = val; var;})
#define STM_LOCAL_WRITE_P(var, val)     ({var = val; var;})


#endif /* STM_H */


/* =============================================================================
 *
 * End of stm.h
 *
 * =============================================================================
 */
