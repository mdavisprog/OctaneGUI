/**

MIT License

Copyright (c) 2022 Mitchell Davis <mdavisprog@gmail.com>

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

#include "Timer.h"
#include "Window.h"

namespace OctaneUI
{

Timer::Timer(int Interval, bool Repeat, Window* InWindow, OnEmptySignature&& Fn)
	: m_Interval(Interval)
	, m_Repeat(Repeat)
	, m_Window(InWindow)
	, m_OnTimeout(std::move(Fn))
{
}

Timer::~Timer()
{
	Stop();
}

Timer& Timer::SetOnTimeout(OnEmptySignature&& Fn)
{
	m_OnTimeout = std::move(Fn);
	return *this;
}

void Timer::Invoke() const
{
	if (m_OnTimeout)
	{
		m_OnTimeout();
	}
}

Timer& Timer::SetInterval(int Interval)
{
	m_Interval = Interval;
	return *this;
}

int Timer::Interval() const
{
	return m_Interval;
}

Timer& Timer::SetRepeat(bool Repeat)
{
	m_Repeat = Repeat;
	return *this;
}

bool Timer::Repeat() const
{
	return m_Repeat;
}

void Timer::Start()
{
	if (m_Window == nullptr)
	{
		return;
	}

	m_Window->StartTimer(this);
}

void Timer::Stop()
{
	if (m_Window == nullptr)
	{
		return;
	}

	m_Window->ClearTimer(this);
}

Timer::Timer()
{
}

}
