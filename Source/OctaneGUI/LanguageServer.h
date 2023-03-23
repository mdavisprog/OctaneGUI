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

#include <string>
#include <unordered_map>
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
    struct Association
    {
    public:
        std::u32string Name;
        std::u32string Path;
        std::vector<std::u32string> Extensions;
    };

    enum class ConnectionStatus
    {
        NotConnected,
        Connecting,
        Connected,
    };

    LanguageServer(Application& App);
    ~LanguageServer();

    LanguageServer& SetSearchEnvironmentPath(bool Value);
    bool SearchEnvironmentPath() const;

    bool Initialize();
    void Shutdown();
    bool IsInitialized() const;
    bool Connect(const char32_t* Name, const char32_t* Path);
    bool ConnectByAssociatedPath(const char32_t* Path);
    bool ConnectByAssociatedExtension(const char32_t* Extension);
    bool Close(const char32_t* Name);
    ConnectionStatus ServerStatus(const char32_t* Name) const;
    ConnectionStatus ServerStatusByPath(const char32_t* Path) const;
    ConnectionStatus ServerStatusByExtension(const char32_t* Extension) const;

    void Process();

    bool OpenDocument(const char32_t* Path);
    void CloseDocument(const char32_t* Path);

    bool DocumentSymbols(const char32_t* Path);

    const Association AssociationByPath(const char32_t* Path) const;
    const Association AssociationByExtension(const char32_t* Extension) const;

private:
    Application& m_App;
    bool m_SearchEnvironmentPath { true };
    bool m_Initialized { false };
    std::vector<Association> m_Associations {};
    std::unordered_map<std::u32string, ConnectionStatus> m_ServerStatus;

#if WITH_LSTALK
    struct LSTalk_Context* m_Context { nullptr };
    std::unordered_map<std::u32string, LSTalk_ServerID> m_Servers;
#endif
};

}
