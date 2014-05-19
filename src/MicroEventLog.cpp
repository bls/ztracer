#include "MicroEventLog.h"
#include "ZTracer.h"
#include <snappy.h>

MicroEventLog::MicroEventLog(ZTracer *ztracer)
    : insStart_(0), ztracer_(ztracer), log_(MAX_LOG_SIZE), osThreadId_(PIN_GetTid())
{
}

void
MicroEventLog::Flush()
{
    ztrace::Event ev = ztracer_->MakeEvent();
    ztrace::MicroEventLog *micro_event_log = ev.mutable_micro_event_log();

    // Compress with snappy
    const std::vector<uint8_t> &bytes = log_.bytes();
    std::string compressed_bytes;
    snappy::Compress((const char *) &bytes[0], bytes.size(), &compressed_bytes);

    // Write out the event
    micro_event_log->set_start_instruction(insStart_);
    micro_event_log->set_data(compressed_bytes);
    ztracer_->WriteEvent(&ev);

    // Update start instruction for this block
    insStart_ = insCounter_.read();

    // Reset log
    log_.clear();
}
