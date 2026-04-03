#include "mutex.h"
#include <cassert>
#include <cstring>

mutex::mutex() {
    InitializeCriticalSection(&cs);
}

void mutex::lock() {
    EnterCriticalSection(&cs);
}

void mutex::unlock() {
    LeaveCriticalSection(&cs);
}