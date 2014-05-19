#include <string>
#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/FileAppender.hh>
#include "PinCompat.h"
#include "Log.h"
#include "ZTracer.h"
#include <unistd.h>
#include <stdio.h>

LOG_INIT("ZTraceMain");

INT32 Usage()
{
  PIN_ERROR( "This Pintool prints a trace of memory addresses\n"
             + KNOB_BASE::StringKnobSummary() + "\n");
  return -1;
}

ZTracer ztracer;

int 
main(int argc, char **argv)
{
#ifdef FUCKING_VERSIONS
    GOOGLE_PROTOBUF_VERIFY_VERSION;
#endif

    if (PIN_Init(argc, argv))  {
        return Usage();
    }

    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::WARN);
    root.removeAllAppenders();
    // TODO: care about memory leak here...
    log4cpp::FileAppender *fileapp = new log4cpp::FileAppender("_", ::dup(fileno(stdout)));
    root.addAppender(fileapp);

    PIN_InitSymbolsAlt(DEBUG_OR_EXPORT_SYMBOLS);
    std::string tracefile = "run.trace." + decstr(PIN_GetPid());

    LOG_WARN("START PID: %d PIN VER: %d LOG: %s", PIN_GetPid(), PIN_BUILD_NUMBER, tracefile.c_str());

    if(!ztracer.OpenLog(tracefile)) {
        LOG_ERROR("Couldn't open log file: %s", tracefile.c_str());
        return 1;
    }
    ztracer.Instrument();
    
    // This also unwinds the tool's stack, so any local (stack 
    // based) variables are lost. That's why ZTracer above is static.
    PIN_StartProgram();
    
    return 0;
}


