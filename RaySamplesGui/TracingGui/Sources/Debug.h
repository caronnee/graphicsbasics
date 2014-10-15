#ifndef __DEBUG__CHECK__
#define __DEBUG__CHECK__

#define DoAssert(val) if ( val==false ) __debugbreak();

#endif