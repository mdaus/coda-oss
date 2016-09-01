#if defined(WIN32)

#include <sys/Backtrace.h>

static const size_t MAX_STACK_ENTRIES = 62;

#include <Windows.h>
#include <DbgHelp.h>

#include <str/Format.h>

namespace
{

//! RAII wrapper for initializing the
//  the Windows symbol functions
class BacktraceHelper
{
public:
    BacktraceHelper()
        : mHandle(GetCurrentProcess())
    {
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        SymInitialize(mHandle, NULL, TRUE);
    }

    ~BacktraceHelper()
    {
        SymCleanup(mHandle);
    }

    HANDLE getHandle() const
    {
        return mHandle;
    }

private:
    HANDLE mHandle;
};

}

std::string sys::getBacktrace()
{
    // Initialize the helper
    // the first time this function runs
    static BacktraceHelper helper;

    void* stackBuffer[MAX_STACK_ENTRIES];
    WORD numFrames = CaptureStackBackTrace(0, MAX_STACK_ENTRIES, stackBuffer, NULL);

    // Create the necessary buffers
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    SYMBOL_INFO *symbol = (SYMBOL_INFO*)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    DWORD displacement;
    IMAGEHLP_LINE64 line;

    std::string trace;
    for (size_t i = 0; i < numFrames; ++i)
    {
        // Parse each stack frame
        DWORD64 address = (DWORD64)stackBuffer[i];
        SymFromAddr(helper.getHandle(), address, NULL, symbol);
        SymGetLineFromAddr64(helper.getHandle(), address, &displacement, &line);

        trace += str::format("\t%s in %s line: %lu address: 0x%0X\n", 
            symbol->Name, line.FileName, line.LineNumber, symbol->Address);
    }

    return trace;
}

#endif
