#ifndef BASE_COMPABILITY_H
#define BASE_COMPABILITY_H

#include <base/inspect.h>

#if SUPPORTED(CPP11)
#define FINAL final
#define OVERRIDE override
#else
#define FINAL
#define OVERRIDE
#endif

#ifdef __APPLE_CC__
#define THREAD_LOCAL __thread
#elif __GNUC__
#if SUPPORTED(CPP11)
#define THREAD_LOCAL thread_local
#else
#define THREAD_LOCAL __thread
#endif // SUPPORTED(CPP11)
#endif // __APPLE_CC__

#endif
