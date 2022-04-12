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

#include "Container.h"

namespace OctaneGUI
{

class TreeItem;
class VerticalContainer;

class Tree : public Container
{
	CLASS(Tree)

public:
	Tree(Window* InWindow);

	std::shared_ptr<Tree> AddChild(const char* Text);

	Tree& SetText(const char* Text);
	const char* GetText() const;

	Tree& SetExpanded(bool Expand);
	bool IsExpanded() const;

	Tree& SetRowSelect(bool RowSelect);
	bool ShouldRowSelect() const;

	Tree& SetMetaData(void* MetaData);
	void* MetaData() const;

	Tree& SetOnSelected(OnTreeSignature&& Fn);

	void Clear();
	bool HasChildren() const;

	virtual std::weak_ptr<Control> GetControl(const Vector2& Point) const override;
	virtual Vector2 DesiredSize() const override;

	virtual void OnLoad(const Json& Root) override;
	virtual void OnPaint(Paint& Brush) const override;
	virtual void OnThemeLoaded() override;

private:
	typedef std::function<void(bool, const std::shared_ptr<TreeItem>&)> OnHoveredTreeItemSignature;
	typedef std::function<void(const std::shared_ptr<TreeItem>&)> OnTreeItemSignature;

	Tree& SetOnHovered(OnHoveredTreeItemSignature&& Fn);
	Tree& SetOnSelected(OnTreeItemSignature&& Fn);

	std::weak_ptr<Control> GetControl(const Vector2& Point, const Rect& RootBounds) const;
	void SetHovered(bool Hovered, const std::shared_ptr<TreeItem>& Item);
	void SetSelected(const std::shared_ptr<TreeItem>& Item);
	void PaintSelection(Paint& Brush, const std::shared_ptr<TreeItem>& Item) const;
	bool IsHidden(const std::shared_ptr<TreeItem>& Item) const;
	void RowSelect(const std::shared_ptr<TreeItem>& Item) const;

	std::shared_ptr<TreeItem> m_Item { nullptr };
	std::shared_ptr<VerticalContainer> m_List { nullptr };

	std::weak_ptr<TreeItem> m_Hovered {};
	OnHoveredTreeItemSignature m_OnHovered { nullptr };

	std::weak_ptr<TreeItem> m_Selected {};
	OnTreeItemSignature m_OnItemSelected { nullptr };

	OnTreeSignature m_OnSelected { nullptr };

	bool m_Expand { false };
	bool m_RowSelect { false };

	void* m_MetaData { nullptr };
};

}
