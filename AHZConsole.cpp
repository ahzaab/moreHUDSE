#include "AHZConsole.h"
#include <stdio.h>  
#include <stdlib.h> 
#include <iostream> 
using namespace std;

static FILE *p_stream;

CAHZDebugConsole::CAHZDebugConsole()
{
}


CAHZDebugConsole::~CAHZDebugConsole()
{
#if _DEBUG
   if (p_stream)
   {
      fclose(p_stream);
      p_stream = nullptr;
      FreeConsole();
   }
#endif
}

CAHZDebugConsole* CAHZDebugConsole::GetInstance()
{
   static CAHZDebugConsole instance;

#if _DEBUG
   if (!p_stream)
   {
      AllocConsole();
      freopen_s(&p_stream, "CONOUT$", "w", stdout);
      printf("Hello console on\n");
      std::cout.clear();
   }
#endif

   return &instance;
}


void CAHZDebugConsole::Log(const char * fmt, ...)
{
#if _DEBUG
   va_list args;
   va_start(args, fmt);
   fprintf(p_stream, fmt, args);
   va_end(args);
#endif
}