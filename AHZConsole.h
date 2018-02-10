#pragma once


class CAHZDebugConsole
{
public:
   ~CAHZDebugConsole();
   static CAHZDebugConsole* GetInstance();

   void Log(const char * fmt, ...);

   // Using as a singleton 
private:
   CAHZDebugConsole();
   //static FILE *p_stream;
};


#if _DEBUG
#define AHZLOG(x,y) CAHZDebugConsole::GetInstance()->Log((x),(y))
#else
#define AHZLOG(x,y)
#endif

