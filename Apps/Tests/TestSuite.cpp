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

#include "TestSuite.h"
#include "OctaneUI/OctaneUI.h"

namespace Tests
{

void TestSuite::Run(OctaneUI::Application& Application)
{
	if (s_Suites->size() == 0)
	{
		delete s_Suites;
		printf("No tests to run.\n");
		return;
	}

	printf("Running %d test suites\n", (int)s_Suites->size());

	for (const TestSuite* TS : *s_Suites)
	{
		printf("\nRunning test suite '%s'\n", TS->m_Name.c_str());

		uint32_t Passed = 0;
		uint32_t Failed = 0;
		for (const std::pair<std::string, OnTestCaseSignature>& Item : TS->m_TestCases)
		{
			Application.GetMainWindow()->Clear();
			bool Result = Item.second(Application);

			if (Result)
			{
				Passed++;
			}
			else
			{
				printf("FAILED: '%s'\n", Item.first.c_str());
				Failed++;
			}
		}

		printf("Completed %d tests. %d Passed %d Failed\n", (int)TS->m_TestCases.size(), Passed, Failed);
	}

	printf("\nAll tests completed\n");

	delete s_Suites;
}

TestSuite::TestSuite(const char* Name, const TestCasesMap& TestCases)
	: m_Name(Name)
	, m_TestCases(TestCases)
{
	if (s_Suites == nullptr)
	{
		s_Suites = new std::vector<TestSuite*>();
	}

	s_Suites->push_back(this);
}

TestSuite::~TestSuite()
{
}

TestSuite::TestSuite()
	: m_Name("")
{
}

std::vector<TestSuite*>* TestSuite::s_Suites = nullptr;

}
