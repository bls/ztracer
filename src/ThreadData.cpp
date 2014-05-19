#include "ThreadData.h"

TLS_KEY ThreadData::micro_event_log_key_;

void
ThreadData::Init()
{
    micro_event_log_key_ = PIN_CreateThreadDataKey(0);
}

void
ThreadData::SetMicroEventLog(MicroEventLog *microEventLog, THREADID threadId)
{
    PIN_SetThreadData(micro_event_log_key_, microEventLog, threadId);
}
