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

Profiler::Event::Event()
{
}

Profiler::Event::Event(const FlyString& Name, int64_t Elapsed)
	: m_Name(Name)
	, m_Elapsed(Elapsed)
{
}

const char* Profiler::Event::Name() const
{
	return m_Name.Data();
}

int64_t Profiler::Event::Elapsed() const
{
	return m_Elapsed;
}

unsigned int Profiler::Event::ExclusiveCount() const
{
	return m_ExclusiveCount;
}

unsigned int Profiler::Event::InclusiveCount() const
{
	return m_InclusiveCount;
}

const std::vector<Profiler::Event>& Profiler::Event::Events() const
{
	return m_Events;
}

Profiler::Sample::Sample()
{
}

Profiler::Sample::Sample(const char* Name, bool Group)
	: m_Name(Name)
	, m_Begin(true)
	, m_Group(Group)
{
	Profiler::Get().BeginSample(*this);
}

Profiler::Sample::~Sample()
{
	if (m_Begin)
	{
		Profiler::Get().EndSample(*this);
	}
}

Profiler::Frame::Frame(bool Begin)
	: m_Begin(Begin)
{
	if (Begin)
	{
		Profiler::Get().BeginFrame();
	}
}

Profiler::Frame::~Frame()
{
	if (m_Begin)
	{
		Profiler::Get().EndFrame();
	}
}

int64_t Profiler::Frame::Elapsed() const
{
	return m_Root.Elapsed();
}

unsigned int Profiler::Frame::InclusiveCount() const
{
	return m_Root.InclusiveCount();
}

unsigned int Profiler::Frame::ExclusiveCount() const
{
	return m_Root.ExclusiveCount();
}

const std::vector<Profiler::Event>& Profiler::Frame::Events() const
{
	return m_Root.Events();
}

void Profiler::Frame::CoalesceEvents()
{
	CoalesceEvents(m_Root);
}

void Profiler::Frame::CoalesceEvents(Profiler::Event& Group)
{
	std::vector<Event> Events;

	bool Found = false;
	unsigned int InclusiveCount = Group.InclusiveCount();
	for (Event& Event_ : Group.m_Events)
	{
		CoalesceEvents(Event_);
		InclusiveCount += Event_.InclusiveCount();

		Found = false;
		for (Event& Item : Events)
		{
			if (Event_.m_Name == Item.m_Name)
			{
				Item.m_Elapsed += Event_.m_Elapsed;
				Item.m_ExclusiveCount++;
				Item.m_InclusiveCount++;
				Found = true;
				break;
			}
		}

		if (!Found)
		{
			Events.push_back(Event_);
		}
	}

	Group.m_Events = std::move(Events);
	Group.m_InclusiveCount = InclusiveCount;
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
		Frame_.CoalesceEvents();
	}
	printf("Finished coalescing.\n");
}

bool Profiler::IsEnabled() const
{
	return m_Enabled;
}

const std::vector<Profiler::Frame>& Profiler::Frames() const
{
	return m_Frames;
}

Profiler::Profiler()
{
}

void Profiler::BeginFrame()
{
	if (!m_Enabled)
	{
		return;
	}

	m_Frames.emplace_back(false);
	Frame& Frame_ = m_Frames.back();
	Frame_.m_Sample.m_Name = "Frame";
	Frame_.m_Sample.m_Group = true;
	BeginSample(Frame_.m_Sample);
}

void Profiler::EndFrame()
{
	if (!m_Enabled || m_Frames.empty())
	{
		return;
	}

	Frame& Frame_ = m_Frames.back();
	EndSample(Frame_.m_Sample);
}

void Profiler::BeginSample(Sample& Sample_)
{
	if (!m_Enabled || m_Frames.empty())
	{
		return;
	}

	Sample_.m_Start = m_Clock.MeasureMS();

	if (Sample_.m_Group)
	{
		m_Groups.emplace_back(Sample_.m_Name.Data(), 0);
	}
}

void Profiler::EndSample(Sample& Sample_)
{
	if (!m_Enabled || m_Frames.empty())
	{
		return;
	}

	int64_t End = m_Clock.MeasureMS();
	int64_t Elapsed = End - Sample_.m_Start;

	Frame& Frame_ = m_Frames.back();

	if (Sample_.m_Group)
	{
		m_Groups.back().m_Elapsed = Elapsed;

		if (m_Groups.size() > 1)
		{
			Event& Dest = m_Groups[m_Groups.size() - 2];
			Dest.m_Events.push_back(std::move(m_Groups.back()));
		}
		else
		{
			Frame_.m_Root = std::move(m_Groups.back());
		}

		m_Groups.pop_back();
	}
	else
	{
		if (m_Groups.empty())
		{
			Frame_.m_Root.m_Elapsed = Elapsed;
		}
		else
		{
			m_Groups.back().m_Events.emplace_back(Sample_.m_Name, Elapsed);
		}
	}
}

}
}
