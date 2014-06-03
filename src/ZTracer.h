#ifndef _ZTRACER_H
#define _ZTRACER_H

#include <fstream>
#include <set>
#include <string>
#include "pin.H"
#include "ZTrace.pb.h"

class ZTracer
{
public:
    // User API
    ZTracer();
    void Init();
    bool OpenLog(std::string path);
    void Instrument();

    // Create & write event messages.  Use this from threads.
    ztrace::Event MakeEvent();
    void WriteEvent(ztrace::Event *msg);

    // Raw memory dumps for debugging / verification.
    void DumpMemoryRegion(ADDRINT addr, ADDRINT endaddr, std::string perms);
    void DumpAllWritableMemory();

public:
    // Pin callbacks    
    VOID OnApplicationStart();
    VOID OnFinish(INT32 code);
    BOOL OnFollowChildProcess(CHILD_PROCESS childProcess);
    VOID OnThreadStart(THREADID threadIndex, CONTEXT *ctx, INT32 code);
    VOID OnThreadFinish(THREADID threadIndex, const CONTEXT *ctx, INT32 code);

private:
    // Internals
    void CloseLog();
    void LogTraceStart();
    void LogTraceEnd();
    void WriteEventUnlocked(const ztrace::Event *msg);
    
private:
    PIN_LOCK output_lock_;
    PIN_LOCK thread_state_lock_;
    UINT32 event_id_;
    std::ofstream output_stream_;
    std::set<THREADID> live_threads_;
};


#endif
