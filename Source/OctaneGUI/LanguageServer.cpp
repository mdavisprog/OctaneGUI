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
#include "Application.h"
#include "Defines.h"
#include "FileSystem.h"
#include "String.h"

namespace OctaneGUI
{

LanguageServer::LanguageServer(Application& App)
    : m_App(App)
{
    // Fill with default associations to be used to connect to a language
    // server.
    m_Associations = {
        {U"clangd", U"clangd", {U".c", U".cpp", U".h", U".hpp"}}
    };
}

LanguageServer::~LanguageServer()
{
}

LanguageServer& LanguageServer::SetSearchEnvironmentPath(bool Value)
{
    m_SearchEnvironmentPath = Value;
    return *this;
}

bool LanguageServer::SearchEnvironmentPath() const
{
    return m_SearchEnvironmentPath;
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

bool LanguageServer::Connect(const char32_t* Name, const char32_t* Path)
{
    if (std::u32string(Name).empty() || std::u32string(Path).empty())
    {
        return false;
    }

    if (ServerStatus(Name) != ConnectionStatus::NotConnected)
    {
        return true;
    }

#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return false;
    }

#ifdef WINDOWS
    const std::u32string FullPath = FileSystem::SetExtension(Path, U"exe");
#else
    const std::u32string FullPath { Path };
#endif

    LSTalk_ConnectParams Params;
    Params.root_uri = nullptr;
    Params.seek_path_env = m_SearchEnvironmentPath ? lstalk_true : lstalk_false;
    LSTalk_ServerID Server = lstalk_connect(m_Context, String::ToMultiByte(FullPath).c_str(), &Params);
    if (Server != LSTALK_INVALID_SERVER_ID)
    {
        m_ServerStatus[Name] = ConnectionStatus::Connecting;
        m_Servers[Name] = Server;
        return true;
    }
#endif

    return false;
}

bool LanguageServer::ConnectByAssociatedExtension(const char32_t* Extension)
{
    const Association Item = GetAssociationByExtension(Extension);
    return Connect(Item.Name.c_str(), Item.Path.c_str());
}

bool LanguageServer::Close(const char32_t* Name)
{
    m_ServerStatus.erase(Name);

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

LanguageServer::ConnectionStatus LanguageServer::ServerStatus(const char32_t* Name) const
{
    if (m_ServerStatus.find(Name) != m_ServerStatus.end())
    {
        return m_ServerStatus.at(Name);
    }

    return ConnectionStatus::NotConnected;
}

LanguageServer::ConnectionStatus LanguageServer::ServerStatusByExtension(const char32_t* Extension) const
{
    const Association Item = GetAssociationByExtension(Extension);
    return ServerStatus(Item.Name.c_str());
}

void LanguageServer::Process()
{
#if WITH_LSTALK
    if (m_Context == NULL)
    {
        return;
    }

    lstalk_process_responses(m_Context);

    for (const std::pair<std::u32string, LSTalk_ServerID>& Item : m_Servers)
    {
        LSTalk_Notification Notification;
        lstalk_poll_notification(m_Context, Item.second, &Notification);

        if (lstalk_get_connection_status(m_Context, Item.second) == LSTALK_CONNECTION_STATUS_CONNECTED)
        {
            m_ServerStatus[Item.first] = ConnectionStatus::Connected;
        }
    }
#endif
}

bool LanguageServer::OpenDocument(const char32_t* Path)
{
    const std::u32string Extension { m_App.FS().Extension(Path) };
    const Association Assoc { GetAssociationByExtension(Extension.c_str()) };

    if (ServerStatus(Assoc.Name.c_str()) != ConnectionStatus::Connected)
    {
        return false;
    }

#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return false;
    }

    if (m_Servers.find(Assoc.Name) == m_Servers.end())
    {
        return false;
    }

    if (lstalk_text_document_did_open(m_Context, m_Servers[Assoc.Name], String::ToMultiByte(Path).c_str()))
    {
        return true;
    }
#endif

    return false;
}

void LanguageServer::CloseDocument(const char32_t* Path)
{
    const std::u32string Extension { m_App.FS().Extension(Path) };
    const Association Assoc { GetAssociationByExtension(Extension.c_str()) };

    if (ServerStatus(Assoc.Name.c_str()) != ConnectionStatus::NotConnected)
    {
        return;
    }

#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return;
    }

    if (m_Servers.find(Assoc.Name) == m_Servers.end())
    {
        return;
    }

    lstalk_text_document_did_close(m_Context, m_Servers[Assoc.Name], String::ToMultiByte(Path).c_str());
#endif
}

const LanguageServer::Association LanguageServer::GetAssociationByExtension(const char32_t* Extension) const
{
    for (const Association& Item : m_Associations)
    {
        for (const std::u32string& Ext : Item.Extensions)
        {
            if (Ext == Extension)
            {
                return Item;
            }
        }
    }

    return LanguageServer::Association {};
}

}
