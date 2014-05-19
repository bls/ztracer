#ifndef THREAD_DATA_H
#define THREAD_DATA_H

#include "pin.H"

class MicroEventLog;

class ThreadData
{
public:
    static void Init();

    static void SetMicroEventLog(MicroEventLog *microEventLog, THREADID threadId);
    
    static inline MicroEventLog *get_micro_event_log(THREADID threadId)
    {
        return static_cast<MicroEventLog *>(PIN_GetThreadData(micro_event_log_key_, threadId));        
    }
    
private:
    static TLS_KEY micro_event_log_key_;
};

#endif
