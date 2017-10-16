#include "PluginProcessInfo.h"
#include <string>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>

using namespace std;

#pragma comment(lib, "psapi") 

CPluginProcessInfo::CPluginProcessInfo(void)
{
}

CPluginProcessInfo::~CPluginProcessInfo(void)
{
}


// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

UInt32 CPluginProcessInfo::GetSKSEBaseAddress( void)
{
    DWORD aProcesses[1024]; 
    DWORD cbNeeded; 
    DWORD cProcesses;
	HANDLE hProcess = NULL;

    // Get the running processes
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return 0;

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);    
	
	for (DWORD i = 0 ; i < cProcesses; i++)
	{
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
								PROCESS_VM_READ,
								FALSE, aProcesses[i] );
		if (hProcess)
		{
			TCHAR Buffer[MAX_PATH];
			if (GetModuleFileNameEx(hProcess, 0, Buffer, MAX_PATH))
			{
				if (_tcsstr(Buffer, _T("TESV.exe")))
				{
					break;
				}
			}
			else
			{
				CloseHandle(hProcess);
				hProcess = NULL;
			}
		}
	}

	if (!hProcess)
	{
		return 0;
	}

	HMODULE hMods[1024];
	cbNeeded = 0;

   // Get a list of all the modules in this process.

    if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.

            if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
                                      sizeof(szModName) / sizeof(TCHAR)))
            {
				if (_tcsstr(szModName, _T("skse_1_9_32.dll")))
				{
					CloseHandle(hProcess);
					return (UInt32)hMods[i];
				}
            }
        }
    }
    
    // Release the handle to the process.

    CloseHandle(hProcess);

    return 0;
}

