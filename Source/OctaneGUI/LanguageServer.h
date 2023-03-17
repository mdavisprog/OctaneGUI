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
    LanguageServer(Application& App);
    ~LanguageServer();

    bool Initialize();
    void Shutdown();
    bool IsInitialized() const;
    bool Connect(const char32_t* Name, const char32_t* Path, bool SearchEnvironmentPaths);
    bool Close(const char32_t* Name);
    void Process();

private:
    Application& m_App;
    bool m_Initialized { false };

#if WITH_LSTALK
    struct LSTalk_Context* m_Context { nullptr };
    std::unordered_map<std::u32string, LSTalk_ServerID> m_Servers;
#endif
};

}
