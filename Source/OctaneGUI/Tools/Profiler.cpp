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

#include "Profiler.h"

#include <cassert>

namespace OctaneGUI
{
namespace Tools
{

Profiler::Sample::Sample(const char* Name)
	: m_Name(Name)
	, m_Begin(true)
{
	Profiler::Get().BeginSample(*this);
}

Profiler::Sample::Sample(const Sample& Other)
	: m_Name(std::move(Other.m_Name))
	, m_Start(Other.m_Start)
	, m_End(Other.m_End)
	, m_TotalElapsed(Other.m_TotalElapsed)
	, m_Calls(Other.m_Calls)
{
}

Profiler::Sample::~Sample()
{
	if (m_Begin)
	{
		Profiler::Get().EndSample(*this);
	}
}

const char* Profiler::Sample::Name() const
{
	return m_Name.c_str();
}

int64_t Profiler::Sample::Elapsed() const
{
	return m_End - m_Start;
}

unsigned int Profiler::Sample::Calls() const
{
	return m_Calls;
}

Profiler::Frame::Frame()
{
	Profiler::Get().BeginFrame(*this);
}

Profiler::Frame::Frame(const Frame& Other)
	: m_Samples(std::move(Other.m_Samples))
	, m_Elapsed(Other.m_Elapsed)
{
}

int64_t Profiler::Frame::Elapsed() const
{
	return m_Elapsed;
}

void Profiler::Frame::CoalesceSamples()
{
	std::vector<Sample> Samples;

	size_t UnmergedSampleCount = m_Samples.size();

	bool Found = false;
	for (Sample& Sample_ : m_Samples)
	{
		m_Elapsed += Sample_.Elapsed();

		Found = false;
		for (Sample& Item : Samples)
		{
			if (Item.m_Name == Sample_.m_Name)
			{
				Item.m_TotalElapsed += Sample_.Elapsed();
				Item.m_Calls++;
				Found = true;
				break;
			}
		}

		if (!Found)
		{
			Samples.push_back(std::move(Sample_));
		}
	}

	m_Samples = std::move(Samples);
}

Profiler& Profiler::Get()
{
	static Profiler g_Profiler {};
	return g_Profiler;
}

void Profiler::Enable()
{
	if (m_Enabled)
	{
		return;
	}

	m_Enabled = true;
	m_Clock.Reset();
	m_Frames.clear();
	printf("Profiler is enabled.\n");
}

void Profiler::Disable()
{
	if (!m_Enabled)
	{
		return;
	}

	m_Enabled = false;
	float Elapsed = m_Clock.Measure();
	printf("Profiler has ended. Elapsed: %f\n", Elapsed);
	printf("Number of frames captured: %d\n", (int)m_Frames.size());
	printf("Coalescing samples...\n");
	uint32_t Count = 1;
	for (Frame& Frame_ : m_Frames)
	{
		Frame_.CoalesceSamples();
	}
	printf("Finished coalescing.\n");
}

Profiler::Profiler()
{
}

void Profiler::BeginFrame(Frame& Frame_)
{
	if (!m_Enabled)
	{
		return;
	}

	m_Frames.push_back(Frame_);
}

void Profiler::BeginSample(Sample& Sample_)
{
	if (!m_Enabled || m_Frames.empty())
	{
		return;
	}

	Sample_.m_Start = m_Clock.MeasureMS();
}

void Profiler::EndSample(Sample& Sample_)
{
	if (!m_Enabled || m_Frames.empty())
	{
		return;
	}

	Sample_.m_End = m_Clock.MeasureMS();
	Frame& Frame_ = m_Frames.back();
	Frame_.m_Samples.push_back(Sample_);
}

}
}
