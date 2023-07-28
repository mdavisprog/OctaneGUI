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
#include <map>
#include <string>
#include <vector>

namespace OctaneGUI
{

class Application;

}

namespace Tests
{

class TestSuite
{
public:
    typedef std::function<bool(OctaneGUI::Application&)> OnTestCaseSignature;
    typedef std::map<std::string, OnTestCaseSignature> TestCasesMap;

    static void Run(OctaneGUI::Application& Application, int Argc, char** Argv);

    TestSuite(const char* Name, const TestCasesMap& TestCases);
    ~TestSuite();

private:
    static bool Run(OctaneGUI::Application& Application, const TestSuite& Suite, uint32_t& Passed, uint32_t& Failed);
    static bool Run(OctaneGUI::Application& Application, const TestSuite& Suite, const std::string& Test);

    TestSuite();

    // Need to make this allocated on the heap due to static initialization order.
    // Would like to register test suites differently but will be using this method for now.
    static std::vector<TestSuite*>* s_Suites;
    static bool s_Verbose;

    std::string m_Name {};
    TestCasesMap m_TestCases;
};

#define TEST_SUITE(Name, Cases) TestSuite Name(#Name, {Cases});
// Disables unreferenced local variable 'Application' for test cases.
// TODO: Find a way to pass Application object into test case to fix this warning.
#if _MSC_VER
    #pragma warning(disable: 4100)
    #define TEST_CASE(Name, Fn) {#Name, [](OctaneGUI::Application& Application) -> bool Fn},
#else
    #define TEST_CASE(Name, Fn) {#Name, [](__attribute__((unused)) OctaneGUI::Application& Application) -> bool Fn},
#endif
#define VERIFY(Condition) if ((Condition) == false) { return false; }

#if __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#define VERIFYF(Condition, Message, ...) if ((Condition) == false) { printf(Message, ##__VA_ARGS__); printf("\n"); return false; }

#if __clang__
    #pragma clang diagnostic pop
#endif

}
