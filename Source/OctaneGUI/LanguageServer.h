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

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#if WITH_LSTALK
    #include "External/lstalk/lstalk.h"
#endif

namespace OctaneGUI
{

class Application;

/// @brief Interface for communicating with a language server.
class LanguageServer
{
public:
    class Server
    {
        friend class LanguageServer;

    public:
        enum class Status
        {
            NotConnected,
            Connecting,
            Connected,
        };

        Server();
        ~Server();

        Status GetStatus() const;
        bool SupportsExtension(const char32_t* Extension) const;
        bool SupportsFile(const char32_t* Path) const;

    private:
        std::u32string m_Name {};
        std::u32string m_Path {};
        std::vector<std::u32string> m_Extensions {};
        Status m_Status { Status::NotConnected };

#if WITH_LSTALK
        LSTalk_ServerID m_Connection { LSTALK_INVALID_SERVER_ID };
#endif
    };

    struct DocumentSymbols
    {
    public:
        struct Symbol
        {
        public:
            std::u32string Name {};
        };

        DocumentSymbols()
        {
        }

        std::u32string URI {};
        std::vector<Symbol> Symbols {};
    };

    struct Notification
    {
    public:
        enum class Type
        {
            None,
            ConnectionStatus,
            DocumentSymbols,
        };

        Notification()
        {
        }

        Type Type_ { Type::None };
        std::variant<DocumentSymbols> Data;
    };

    typedef std::function<void(const Notification&, const std::shared_ptr<Server>&)> OnNotificationSignature;
    typedef unsigned int ListenerID;
    static const ListenerID INVALID_LISTENER_ID = 0;

    LanguageServer();
    ~LanguageServer();

    LanguageServer& SetSearchEnvironmentPath(bool Value);
    bool SearchEnvironmentPath() const;

    bool Initialize();
    void Shutdown();
    bool IsInitialized() const;
    void AddServer(const char32_t* Name, const char32_t* Path, const std::vector<std::u32string>& Extensions);
    bool Connect(const char32_t* Name, const char32_t* Path, const std::vector<std::u32string>& Extensions);
    bool ConnectFromFilePath(const char32_t* Path);
    bool Close(const char32_t* Name);
    Server::Status ServerStatus(const char32_t* Name) const;
    Server::Status ServerStatusByFilePath(const char32_t* Path) const;
    Server::Status ServerStatusByExtension(const char32_t* Extension) const;

    ListenerID RegisterListener(OnNotificationSignature&& Fn);
    LanguageServer& UnregisterListener(ListenerID ID);

    void Process();

    bool OpenDocument(const char32_t* Path);
    void CloseDocument(const char32_t* Path);
    bool GetDocumentSymbols(const char32_t* Path);

private:
    struct Listener
    {
    public:
        ListenerID ID { 0 };
        OnNotificationSignature Callback {};
    };

    std::shared_ptr<Server> GetServer(const char32_t* Name) const;
    std::shared_ptr<Server> GetServerByFilePath(const char32_t* Path) const;
    std::shared_ptr<Server> GetServerByExtension(const char32_t* Extension) const;
    void Broadcast(const Notification& Data, const std::shared_ptr<Server>& Target) const;
    Notification CreateNotification(Notification::Type Type) const;

    bool m_SearchEnvironmentPath { true };
    bool m_Initialized { false };
    std::vector<std::shared_ptr<Server>> m_Servers {};
    std::vector<Listener> m_Listeners {};
    ListenerID m_ListenerID { 0 };

#if WITH_LSTALK
    struct LSTalk_Context* m_Context { nullptr };
#endif
};

}
