#ifndef BASE_INSPECT_H
#define BASE_INSPECT_H

#if __APPLE_CC__ || (__GNUC__ >= 4 && __GNUC_MINOR__ >= 7)
#define CPP11_SUPPORTED 1
#endif

#if ((BOOST_VERSION / 100000) >= 1) && (((BOOST_VERSION / 100) % 1000) >= 50)
#define BOOST_SCOPE_EXIT_ALL_SUPPORTED 1
#endif

#define SUPPORTED(x) x ## _ ## SUPPORTED

#define ENABLED(x) x ## _ ## ENABLED

#endif
