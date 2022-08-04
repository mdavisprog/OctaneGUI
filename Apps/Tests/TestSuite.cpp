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
#include "OctaneGUI/OctaneGUI.h"

namespace Tests
{

void TestSuite::Run(OctaneGUI::Application& Application, int Argc, char** Argv)
{
    if (s_Suites->size() == 0)
    {
        delete s_Suites;
        printf("No tests to run.\n");
        return;
    }

    std::string SuiteName;
    if (Argc > 1)
    {
        for (int I = 1; I < Argc; I++)
        {
            if (std::string(Argv[I]) == "Verbose")
            {
                s_Verbose = true;
            }
        }

        SuiteName = Argv[Argc - 1];
        SuiteName = SuiteName == "Verbose" ? "" : SuiteName;
    }

    if (!SuiteName.empty())
    {
        bool Found = false;
        for (const TestSuite* TS : *s_Suites)
        {
            if (SuiteName == TS->m_Name)
            {
                uint32_t Passed = 0;
                uint32_t Failed = 0;
                Run(Application, *TS, Passed, Failed);
                Found = true;
                break;
            }
        }

        if (!Found)
        {
            printf("Failed to find test suite '%s'.\n", SuiteName.c_str());
        }
    }
    else
    {
        uint32_t Passed = 0;
        uint32_t Failed = 0;
        uint32_t TotalPassed = 0;
        uint32_t TotalFailed = 0;

        OctaneGUI::Clock Clock;
        printf("Running %d test suites\n", (int)s_Suites->size());
        for (const TestSuite* TS : *s_Suites)
        {
            Run(Application, *TS, Passed, Failed);
            TotalPassed += Passed;
            TotalFailed += Failed;
        }

        printf("\n%d tests completed in %f seconds. Pass/Fail: %d/%d.\n", TotalPassed + TotalFailed, Clock.Measure(), TotalPassed, TotalFailed);
    }

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

bool TestSuite::Run(OctaneGUI::Application& Application, const TestSuite& Suite, uint32_t& Passed, uint32_t& Failed)
{
    OctaneGUI::Clock Clock;
    printf("\nRunning test suite '%s'\n", Suite.m_Name.c_str());

    Passed = 0;
    Failed = 0;
    for (const std::pair<std::string, OnTestCaseSignature>& Item : Suite.m_TestCases)
    {
        Application.GetMainWindow()->Clear();
        bool Result = Item.second(Application);

        if (s_Verbose)
        {
            printf("Running test case '%s'\n", Item.first.c_str());
        }

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

    printf("Completed %d tests in %f seconds. %d Passed %d Failed\n", 
        (int)Suite.m_TestCases.size(),
        Clock.Measure(),
        Passed,
        Failed
    );

    return true;
}

TestSuite::TestSuite()
{
}

std::vector<TestSuite*>* TestSuite::s_Suites = nullptr;
bool TestSuite::s_Verbose { false };

}
