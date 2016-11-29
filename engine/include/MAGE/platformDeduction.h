#if defined(_MSC_VER)
#define PLATFORM_WINDOWS // MSC only runs on windows
#define PLATFORM_STRING std::string("Windows")

#if defined(MAGEDLL_EXPORT)
#define MAGEDLL __declspec(dllexport)
#else
#define MAGEDLL __declspec(dllimport)
#endif
#elif defined(__GNUC__)
#if defined(WIN32)
#define PLATFORM_WINDOWS
#define PLATFORM_STRING std::string("Windows")
#endif
#if defined(__linux__)
#define PLATFORM_LINUX
#define PLATFORM_STRING std::string("Linux")
#endif

#if defined(MAGEDLL_EXPORT)
#define MAGEDLL __attribute__ ((visibility ("default")))
#else
#define MAGEDLL
#endif
#else
#error "Please use either GCC or Microsoft Visual C++ (preferred) to compile this."
#endif