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

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace OctaneGUI
{
class Control;

/// @brief Object that contains a list of controls that has a valid ID.
///
/// Any controls that has an ID defined will be collected and put into this
/// list. The control can then be retrieved by the fully qualified ID path
/// which can allow for setting up callbacks and modify other settings if
/// desired.
class ControlList
{
public:
    ControlList();
    ~ControlList();

    void AddControl(const std::shared_ptr<Control>& InControl);
    void AddControls(const std::vector<std::shared_ptr<Control>>& Controls);
    bool Contains(const char* ID) const;
    std::weak_ptr<Control> Get(const char* ID) const;
    std::vector<std::string> IDs() const;

    template <class T>
    std::shared_ptr<T> To(const char* ID) const
    {
        std::shared_ptr<T> Result;
        std::weak_ptr<Control> Item = Get(ID);
        if (!Item.expired())
        {
            Result = std::dynamic_pointer_cast<T>(Item.lock());
        }

        return Result;
    }

private:
    typedef std::unordered_map<std::string, std::weak_ptr<Control>> Map;

    Map m_Controls;
};

}
