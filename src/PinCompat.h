#ifndef _PIN_INC_H
#define _PIN_INC_H

#include "pin.H"

// Between pin 2.12 -> 2.13, locking primitives were renamed.
#if PIN_PRODUCT_VERSION_MAJOR == 2
#if PIN_PRODUCT_VERSION_MINOR < 13
    
#define PIN_InitLock InitLock
#define PIN_GetLock GetLock
#define PIN_ReleaseLock ReleaseLock

#endif 
#endif


#endif

