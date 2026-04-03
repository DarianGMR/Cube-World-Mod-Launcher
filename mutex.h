#ifndef GCC_MUTEX_H
#define GCC_MUTEX_H

#include <windows.h>

// Mutex simple para sincronización de threads
class mutex {
private:
    CRITICAL_SECTION cs;
public:
    mutex();
    void lock();
    void unlock();
};

#endif // GCC_MUTEX_H