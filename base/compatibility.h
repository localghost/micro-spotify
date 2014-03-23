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

#endif
