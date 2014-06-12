// this file should be included *last* in every .cpp

#ifndef EMPTY_PROJECT_DEBUG_H
#define EMPTY_PROJECT_DEBUG_H


// in debug builds, enable file/line tracking for memory leaks
#if defined(_DEBUG) && defined(_MSC_VER)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif


#endif // EQRAY_DEBUG_H
