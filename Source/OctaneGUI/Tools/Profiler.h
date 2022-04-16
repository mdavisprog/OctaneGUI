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

#pragma once

#include "../Clock.h"

#include <string>
#include <vector>

namespace OctaneGUI
{
namespace Tools
{

class Profiler
{
public:
	class Sample
	{
		friend Profiler;

	public:
		Sample(const char* Name);
		Sample(const Sample& Other);
		~Sample();

		const char* Name() const;
		int64_t Elapsed() const;
		int64_t TotalElapsed() const;
		unsigned int Calls() const;

	private:
		std::string m_Name {};
		int64_t m_Start { 0 };
		int64_t m_End { 0 };
		int64_t m_TotalElapsed { 0 };
		unsigned int m_Calls { 1 };
		bool m_Begin { false };
	};

	class Frame
	{
		friend Profiler;

	public:
		Frame();
		Frame(const Frame& Other);

		int64_t Elapsed() const;

	private:
		void CoalesceSamples();

		std::vector<Sample> m_Samples {};
		int64_t m_Elapsed { 0 };
	};

	static Profiler& Get();

	void Enable();
	void Disable();

private:
	Profiler();

	void BeginFrame(Frame& Frame_);

	void BeginSample(Sample& Sample_);
	void EndSample(Sample& Sample_);

	bool m_Enabled { false };
	std::vector<Frame> m_Frames {};
	Clock m_Clock {};
};

}

}
