/**

MIT License

Copyright (c) 2022-2023 Mitchell Davis <mdavisprog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Network.h"
#include "Defines.h"

#ifdef WINDOWS
    #include <WinSock2.h>
    #pragma comment(lib, "Ws2_32.lib")
#endif

#include <cstdio>

namespace OctaneGUI
{

Network::Network()
{
}

bool Network::Initialize()
{
    if (m_Initialized)
    {
        return true;
    }

#ifdef WINDOWS
    WSADATA Data {};
    int Result = WSAStartup(MAKEWORD(2, 2), &Data);
    if (Result != ERROR_SUCCESS)
    {
        switch (Result)
        {
        case WSASYSNOTREADY:
            printf("The underlying network subsystem is not ready for network communication.");
            break;
        
        case WSAVERNOTSUPPORTED:
            printf("The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.");
            break;
        
        case WSAEINPROGRESS:
            printf("A blocking Windows Sockets 1.1 operation is in progress.");
            break;
        
        case WSAEPROCLIM:
            printf("A limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
            break;
        
        case WSAEFAULT:
            printf("The lpWSAData parameter is not a valid pointer.");
            break;
        
        default: break;
        }

        return false;
    }

    m_Initialized = true;
#endif

    return m_Initialized;
}

void Network::Shutdown()
{
    if (!m_Initialized)
    {
        return;
    }

#ifdef WINDOWS
    WSACleanup();
#endif

    m_Initialized = false;
}

bool Network::Initialized() const
{
    return m_Initialized;
}

}
