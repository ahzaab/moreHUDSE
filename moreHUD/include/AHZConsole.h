#pragma once

namespace Debug
{
    class CAHZDebugConsole
    {
    public:
        ~CAHZDebugConsole();
        CAHZDebugConsole();

    private:
        static FILE* p_stream;
    };

}
