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

#include "Vector2.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace OctaneGUI
{

class Texture
{
public:
    typedef std::function<uint32_t(const std::vector<uint8_t>&, uint32_t, uint32_t)> OnLoadSignature;

    static void SetOnLoad(OnLoadSignature Fn);
    static std::shared_ptr<Texture> Load(const std::vector<uint8_t>& Data, uint32_t Width, uint32_t Height);
    static std::shared_ptr<Texture> Load(const char* Path);
    static std::shared_ptr<Texture> LoadPNG(const char* Path);
    static std::shared_ptr<Texture> LoadSVG(const char* Path);
    static std::shared_ptr<Texture> LoadSVG(const char* Path, uint32_t Width, uint32_t Height);
    static std::vector<uint8_t> LoadSVGData(const char* Path, uint32_t Width, uint32_t Height);

    Texture();
    ~Texture();

    bool IsValid() const;
    uint32_t GetID() const;
    Vector2 GetSize() const;
    const char* Path() const;

private:
    static OnLoadSignature s_OnLoad;

    uint32_t m_ID { 0 };
    Vector2 m_Size {};
    std::string m_Path {};
};

}
