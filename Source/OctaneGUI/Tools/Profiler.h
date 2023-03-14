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

#include "../Clock.h"
#include "../FlyString.h"

#include <vector>

namespace OctaneGUI
{
namespace Tools
{

class Profiler
{
public:
    class Event
    {
        friend Profiler;

    public:
        Event();
        Event(const FlyString& Name, int64_t Elapsed);

        const char* Name() const;
        int64_t Elapsed() const;
        unsigned int ExclusiveCount() const;
        unsigned int InclusiveCount() const;
        const std::vector<Event>& Events() const;

    private:
        FlyString m_Name {};
        int64_t m_Elapsed { 0 };
        unsigned int m_ExclusiveCount { 1 };
        unsigned int m_InclusiveCount { 1 };
        std::vector<Event> m_Events {};
    };

    class Sample
    {
        friend Profiler;

    public:
        Sample();
        Sample(const char* Name, bool Group);
        ~Sample();

    private:
        FlyString m_Name {};
        int64_t m_Start { 0 };
        bool m_Begin { false };
        bool m_Group { false };
    };

    class Frame
    {
        friend Profiler;

    public:
        Frame(bool Begin = true);
        ~Frame();

        int64_t Elapsed() const;
        unsigned int InclusiveCount() const;
        unsigned int ExclusiveCount() const;
        const std::vector<Event>& Events() const;

    private:
        void CoalesceEvents();
        void CoalesceEvents(Event& Group);

        Sample m_Sample {};
        Event m_Root {};
        bool m_Begin { false };
    };

    static Profiler& Get();

    void Enable();
    void Disable();
    bool IsEnabled() const;

    const std::vector<Frame>& Frames() const;

private:
    Profiler();

    void BeginFrame();
    void EndFrame();

    void BeginSample(Sample& Sample_);
    void EndSample(Sample& Sample_);

    bool m_Enabled { false };
    std::vector<Frame> m_Frames {};
    std::vector<Event> m_Groups {};
    Clock m_Clock {};
};

}

}
