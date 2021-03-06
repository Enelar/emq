#ifndef _ED_DEF_H_
#define _ED_DEF_H_

#if _DEBUG
//! @define Is library should be compiled in debug mode
  #define _DEBUG_ 1
#else
  #define _DEBUG_ 0
#endif

#define QUOTEME(x) #x
#define TOSTRING(x) QUOTEME(x)

namespace ed
{
  static bool memleak_debug_enabled = false;
};


// Memory leaks detection
#ifndef _MEMLEAK_DETECT_ENABLED
  #define _ED_DETECT_MEMORY_LEAKS_
  #define _MEMLEAK_DETECT_ENABLED
  #if _DEBUG_
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
    #define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define BREAK_ON_MEMORY_LEAK(x) _CrtSetBreakAlloc(x)

    static class dbgMEMORY
    {
    public:
      dbgMEMORY()
      {
        if (ed::memleak_debug_enabled)
          return;
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF    | _CRTDBG_LEAK_CHECK_DF |
                       _CRTDBG_CHECK_ALWAYS_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
        ed::memleak_debug_enabled = true;

        NEW int(
          'l' +
          ('e' << 8) +
          ('a' << 16) +
          ('k' << 24)
          );
      }
    } _MEMORY;
  #else
    #define BREAK_ON_MEMORY_LEAK(x) 
    #define NEW new
  #endif // _DEBUG_
#endif // MEMLEAK_DETECT_ENABLE

#ifdef _ED_COMPILLED_LOCALLY_
#define _ED_DLL_EXPORT_ 
//__declspec(dllexport)
#else
#define _ED_DLL_EXPORT_
//__declspec(dllimport)
#endif

template<typename B, typename A>
B convert( const A &a )
{
  return static_cast<B>(a);
}

template<typename B, typename A, typename A1>
B convert<>( const A &, const A1 & );

#endif