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

#include "LanguageServer.h"
#include "String.h"

namespace OctaneGUI
{

LanguageServer::LanguageServer()
{
}

LanguageServer::~LanguageServer()
{
}

bool LanguageServer::Initialize()
{
    if (m_Initialized)
    {
        return m_Initialized;
    }

#if WITH_LSTALK
    m_Context = lstalk_init();
    if (m_Context != nullptr)
    {
        lstalk_set_client_info(m_Context, "CodeEdit", "1");
        m_Initialized = true;
    }
#endif
    return m_Initialized;
}

void LanguageServer::Shutdown()
{
#if WITH_LSTALK
    lstalk_shutdown(m_Context);
#endif
    m_Initialized = false;
}

bool LanguageServer::IsInitialized() const
{
    return m_Initialized;
}

bool LanguageServer::Connect(const char32_t* Name, const char32_t* Path, bool SearchEnvironmentPaths)
{
    bool Result = false;
#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return Result;
    }

    LSTalk_ConnectParams Params;
    Params.root_uri = nullptr;
    Params.seek_path_env = SearchEnvironmentPaths ? lstalk_true : lstalk_false;
    LSTalk_ServerID Server = lstalk_connect(m_Context, String::ToMultiByte(Path).c_str(), &Params);
    if (Server != LSTALK_INVALID_SERVER_ID)
    {
        m_Servers[Name] = Server;
        Result = true;
    }
#endif
    return Result;
}

bool LanguageServer::Close(const char32_t* Name)
{
#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return false;
    }

    if (m_Servers.find(Name) != m_Servers.end())
    {
        lstalk_close(m_Context, m_Servers[Name]);
        m_Servers.erase(Name);
        return true;
    }
#endif

    return false;
}

}
