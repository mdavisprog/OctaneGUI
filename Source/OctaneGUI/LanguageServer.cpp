/**

MIT License

Copyright (c) 2022-2024 Mitchell Davis <mdavisprog@gmail.com>

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

LanguageServer::Server::Server()
{
}

LanguageServer::Server::~Server()
{
}

LanguageServer::Server::Status LanguageServer::Server::GetStatus() const
{
    return m_Status;
}

bool LanguageServer::Server::SupportsExtension(const char32_t* Extension) const
{
    for (const std::u32string& Item : m_Extensions)
    {
        if (Item == Extension)
        {
            return true;
        }
    }

    return false;
}

bool LanguageServer::Server::SupportsFile(const char32_t* Path) const
{
    return SupportsExtension(FileSystem::Extension(Path).c_str());
}

LanguageServer::LanguageServer()
{
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
    ClearListeners();
#if WITH_LSTALK
    lstalk_shutdown(m_Context);
    m_Context = nullptr;
#endif
    m_Initialized = false;
    m_Servers.clear();
}

bool LanguageServer::IsInitialized() const
{
    return m_Initialized;
}

void LanguageServer::AddServer(const char32_t* Name, const char32_t* Path, const std::vector<std::u32string>& Extensions)
{
    if (GetServer(Name))
    {
        return;
    }

    std::shared_ptr<Server> Item = std::make_shared<Server>();
    Item->m_Name = Name;
    Item->m_Path = Path;
    Item->m_Extensions = Extensions;
    m_Servers.push_back(Item);
}

bool LanguageServer::Connect(const char32_t* Name, const char32_t* Path, const std::vector<std::u32string>& Extensions)
{
    if (std::u32string(Name).empty() || std::u32string(Path).empty())
    {
        return false;
    }

    if (ServerStatus(Name) != Server::Status::NotConnected)
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
    LSTalk_ServerID Connection = lstalk_connect(m_Context, String::ToMultiByte(FullPath).c_str(), &Params);
    if (Connection != LSTALK_INVALID_SERVER_ID)
    {
        AddServer(Name, Path, Extensions);
        std::shared_ptr<Server> Item = GetServer(Name);
        Item->m_Status = Server::Status::Connecting;
        Item->m_Connection = Connection;
        m_Servers.push_back(Item);
        return true;
    }
#else
    (void)Extensions;
#endif

    return false;
}

bool LanguageServer::ConnectFromFilePath(const char32_t* Path)
{
    const std::shared_ptr<Server> Item = GetServerByFilePath(Path);

    if (!Item)
    {
        return false;
    }

    return Connect(Item->m_Name.c_str(), Item->m_Path.c_str(), Item->m_Extensions);
}

bool LanguageServer::Close(const char32_t* Name)
{
#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return false;
    }

    for (std::vector<std::shared_ptr<Server>>::const_iterator It = m_Servers.begin(); It != m_Servers.end(); ++It)
    {
        const std::shared_ptr<Server>& Item = *It;
        if (Item->m_Name == Name)
        {
            lstalk_close(m_Context, Item->m_Connection);
            m_Servers.erase(It);
            break;
        }
    }
#else
    (void)Name;
#endif

    return false;
}

LanguageServer::Server::Status LanguageServer::ServerStatus(const char32_t* Name) const
{
    const std::shared_ptr<Server> Item = GetServer(Name);

    if (Item)
    {
        return Item->m_Status;
    }

    return Server::Status::NotConnected;
}

LanguageServer::Server::Status LanguageServer::ServerStatusByFilePath(const char32_t* Path) const
{
    const std::shared_ptr<Server> Item = GetServerByFilePath(Path);

    if (Item)
    {
        return Item->m_Status;
    }

    return Server::Status::NotConnected;
}

LanguageServer::Server::Status LanguageServer::ServerStatusByExtension(const char32_t* Extension) const
{
    const std::shared_ptr<Server> Item = GetServerByExtension(Extension);

    if (Item)
    {
        return Item->m_Status;
    }

    return Server::Status::NotConnected;
}

LanguageServer::ListenerID LanguageServer::RegisterListener(OnNotificationSignature&& Fn)
{
    ListenerID ID = ++m_ListenerID;
    m_Listeners.push_back({ ID, std::move(Fn) });
    return ID;
}

LanguageServer& LanguageServer::UnregisterListener(ListenerID ID)
{
    if (ID == INVALID_LISTENER_ID)
    {
        return *this;
    }

    for (std::vector<Listener>::const_iterator It = m_Listeners.begin(); It != m_Listeners.end(); ++It)
    {
        if ((*It).ID == ID)
        {
            m_Listeners.erase(It);
            break;
        }
    }

    return *this;
}

LanguageServer& LanguageServer::ClearListeners()
{
    for (const Listener& Item : m_Listeners)
    {
        UnregisterListener(Item.ID);
    }

    m_Listeners.clear();

    return *this;
}

#if WITH_LSTALK
static LanguageServer::Server::Status ToStatus(LSTalk_ConnectionStatus Status)
{
    switch (Status)
    {
    case LSTALK_CONNECTION_STATUS_CONNECTING: return LanguageServer::Server::Status::Connecting;
    case LSTALK_CONNECTION_STATUS_CONNECTED: return LanguageServer::Server::Status::Connected;
    default: break;
    }

    return LanguageServer::Server::Status::NotConnected;
}
#endif

void LanguageServer::Process()
{
#if WITH_LSTALK
    if (m_Context == NULL)
    {
        return;
    }

    lstalk_process_responses(m_Context);

    for (const std::shared_ptr<Server>& Item : m_Servers)
    {
        LSTalk_ServerID Connection = Item->m_Connection;

        LSTalk_Notification Notification;
        lstalk_poll_notification(m_Context, Connection, &Notification);

        LanguageServer::Server::Status Status = ToStatus(lstalk_get_connection_status(m_Context, Connection));
        if (Status != Item->m_Status)
        {
            Item->m_Status = Status;
            Broadcast(CreateNotification(Notification::Type::ConnectionStatus), Item);
        }

        switch (Notification.type)
        {
        case LSTALK_NOTIFICATION_TEXT_DOCUMENT_SYMBOLS:
        {
            LanguageServer::Notification Notify = CreateNotification(Notification::Type::DocumentSymbols);
            DocumentSymbols* Symbols = std::get_if<DocumentSymbols>(&Notify.Data);
            if (Symbols != nullptr)
            {
                Symbols->URI = String::ToUTF32(Notification.data.document_symbols.uri);
                for (int I = 0; I < Notification.data.document_symbols.symbols_count; I++)
                {
                    LSTalk_DocumentSymbol* Symbol = &Notification.data.document_symbols.symbols[I];

                    DocumentSymbols::Symbol NewSymbol {};
                    NewSymbol.Name = String::ToUTF32(Symbol->name);
                    Symbols->Symbols.push_back(NewSymbol);
                }

                Broadcast(Notify, Item);
            }
        }
        break;

        default: break;
        }
    }
#endif
}

bool LanguageServer::OpenDocument(const char32_t* Path)
{
    if (ServerStatusByFilePath(Path) != Server::Status::Connected)
    {
        return false;
    }

#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return false;
    }

    const std::shared_ptr<Server> Item = GetServerByFilePath(Path);

    if (!Item)
    {
        return false;
    }

    if (lstalk_text_document_did_open(m_Context, Item->m_Connection, String::ToMultiByte(Path).c_str()))
    {
        return true;
    }
#endif

    return false;
}

void LanguageServer::CloseDocument(const char32_t* Path)
{
    if (ServerStatusByFilePath(Path) != Server::Status::Connected)
    {
        return;
    }

#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return;
    }

    const std::shared_ptr<Server> Item = GetServerByFilePath(Path);

    if (!Item)
    {
        return;
    }

    lstalk_text_document_did_close(m_Context, Item->m_Connection, String::ToMultiByte(Path).c_str());
#endif
}

bool LanguageServer::GetDocumentSymbols(const char32_t* Path)
{
    if (ServerStatusByFilePath(Path) != Server::Status::Connected)
    {
        return false;
    }

#if WITH_LSTALK
    if (m_Context == nullptr)
    {
        return false;
    }

    const std::shared_ptr<Server> Item = GetServerByFilePath(Path);

    if (!Item)
    {
        return false;
    }

    if (lstalk_text_document_symbol(m_Context, Item->m_Connection, String::ToMultiByte(Path).c_str()))
    {
        return true;
    }
#endif

    return false;
}

std::shared_ptr<LanguageServer::Server> LanguageServer::GetServer(const char32_t* Name) const
{
    for (const std::shared_ptr<Server>& Item : m_Servers)
    {
        if (Item->m_Name == Name)
        {
            return Item;
        }
    }

    return nullptr;
}

std::shared_ptr<LanguageServer::Server> LanguageServer::GetServerByFilePath(const char32_t* Path) const
{
    const std::u32string Extension { FileSystem::Extension(Path) };
    return GetServerByExtension(Extension.c_str());
}

std::shared_ptr<LanguageServer::Server> LanguageServer::GetServerByExtension(const char32_t* Extension) const
{
    for (const std::shared_ptr<Server>& Item : m_Servers)
    {
        if (Item->SupportsExtension(Extension))
        {
            return Item;
        }
    }

    return nullptr;
}

void LanguageServer::Broadcast(const Notification& Data, const std::shared_ptr<Server>& Target) const
{
    for (const Listener& Listener_ : m_Listeners)
    {
        Listener_.Callback(Data, Target);
    }
}

LanguageServer::Notification LanguageServer::CreateNotification(Notification::Type Type) const
{
    Notification Result;
    Result.Type_ = Type;
    return Result;
}

}
