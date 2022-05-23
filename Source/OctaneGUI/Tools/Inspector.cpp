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

#include "Inspector.h"
#include "../Application.h"
#include "../Controls/Container.h"
#include "../Controls/ControlList.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/ScrollableViewControl.h"
#include "../Controls/Splitter.h"
#include "../Controls/Tree.h"
#include "../Json.h"
#include "../Paint.h"
#include "../Window.h"
#include "Properties.h"

#include <sstream>

namespace OctaneGUI
{
namespace Tools
{

class InspectorProxy : public Control
{
	CLASS(InspectorProxy)

public:
	typedef std::function<void(const std::weak_ptr<Control>&)> OnSelectedSignature;

	InspectorProxy(Window* InWindow)
		: Control(InWindow)
	{
		SetExpand(Expand::Both);
	}

	InspectorProxy& SetRoot(const std::weak_ptr<Container>& Root)
	{
		m_Root = Root;
		return *this;
	}

	InspectorProxy& SetOnSelected(OnSelectedSignature&& Fn)
	{
		m_OnSelected = std::move(Fn);
		return *this;
	}

	void Clear()
	{
		m_Hovered.reset();
		m_Selected.reset();
	}

	virtual void OnMouseMove(const Vector2& Position) override
	{
		if (m_Root.expired())
		{
			return;
		}

		std::weak_ptr<Control> Hovered = GetControl(Position, m_Root.lock());
		if (m_Hovered.lock() != Hovered.lock())
		{
			m_Hovered = Hovered;
			Invalidate();
		}
	}

	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override
	{
		if (m_Hovered.expired())
		{
			return false;
		}

		if (Button != Mouse::Button::Left)
		{
			return false;
		}

		m_Selected = m_Hovered;

		if (m_OnSelected)
		{
			m_OnSelected(m_Selected);
		}

		return true;
	}

	virtual void OnPaint(Paint& Brush) const override
	{
		if (!m_Hovered.expired())
		{
			std::shared_ptr<Control> Hovered = m_Hovered.lock();
			Brush.RectangleOutline(Hovered->GetAbsoluteBounds(), Color(255, 255, 0, 255));
		}

		if (!m_Selected.expired())
		{
			std::shared_ptr<Control> Selected = m_Selected.lock();
			Brush.RectangleOutline(Selected->GetAbsoluteBounds(), Color(0, 255, 0, 255));
		}
	}

private:
	std::weak_ptr<Control> GetControl(const Vector2& Point, const std::shared_ptr<Container>& Root) const
	{
		std::weak_ptr<Control> Result;

		if (!Root)
		{
			return Result;
		}

		const std::vector<std::shared_ptr<Control>>& Controls = Root->Controls();
		for (int I = Controls.size() - 1; I >= 0; I--)
		{
			const std::shared_ptr<Control>& Item = Controls[I];
			const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
			if (ItemContainer)
			{
				Result = GetControl(Point, ItemContainer);

				if (Result.expired() && ItemContainer->Contains(Point))
				{
					Result = ItemContainer;
				}
			}
			else if (Item->Contains(Point) && Item.get() != this)
			{
				Result = Item;
			}

			if (!Result.expired())
			{
				break;
			}
		}

		return Result;
	}

	std::weak_ptr<Container> m_Root {};
	std::weak_ptr<Control> m_Hovered {};
	std::weak_ptr<Control> m_Selected {};
	OnSelectedSignature m_OnSelected { nullptr };
};

Inspector& Inspector::Get()
{
	return s_Inspector;
}

void Inspector::Inspect(Window* Target)
{
	if (m_Window.expired())
	{
		std::stringstream Stream;
		Stream << "{\"Title\": \"Inspecting "
			   << Target->GetTitle()
			   << "\","
			   << "\"Width\": 600,"
			   << "\"Height\": 300,"
			   << "\"Body\": {\"Controls\": ["
			   << "{\"Type\": \"Panel\", \"Expand\": \"Both\"},"
			   << "{\"ID\": \"Root\", \"Type\": \"Splitter\", \"Orientation\": \"Vertical\", \"Expand\": \"Both\"}"
			   << "]}}";

		ControlList List;
		std::shared_ptr<Window> NewWindow = Target->App().NewWindow("Inspector", Stream.str().c_str(), List);
		NewWindow->SetOnClose([this](Window& InWindow) -> void
			{
				Close();
			});
		m_Window = NewWindow;
		m_Root = List.To<Splitter>("Root");

		m_Proxy = std::make_shared<InspectorProxy>(NewWindow.get());
		m_Proxy->SetOnSelected([this](const std::weak_ptr<Control>& Selected) -> void
			{
				if (Selected.expired() || m_Tree.expired())
				{
					return;
				}

				std::shared_ptr<Control> Selected_ = Selected.lock();
				ParseProperty(Selected_.get());
				std::shared_ptr<Tree> Tree_ = m_Tree.lock();
				Tree_->SetExpandedAll(false);
				ExpandTree(m_Tree.lock(), Selected_.get());
			});
	}
	else
	{
		m_Window.lock()->SetTitle((std::string("Inspecting ") + Target->GetTitle()).c_str());
	}

	if (m_Window.lock()->IsVisible())
	{
		return;
	}

	m_Target = Target;
	m_Target->SetOnClose([this](Window&) -> void
		{
			Close();
		});
	Target->App().DisplayWindow("Inspector");

	m_Proxy
		->SetRoot(Target->GetContainer())
		.SetSize(Target->GetSize())
		->SetWindow(Target);
	Target->GetContainer()->InsertControl(m_Proxy);

	std::shared_ptr<Splitter> Split = m_Root.lock();
	Split->First()->ClearControls();
	Split->Second()->ClearControls();
	Populate();
}

Inspector Inspector::s_Inspector {};

Inspector::Inspector()
{
}

static void PopulateTree(const std::shared_ptr<Tree>& Root, const std::shared_ptr<Container>& Target)
{
	Root->SetText(Target->GetType());
	Root->SetMetaData(Target.get());

	for (const std::shared_ptr<Control>& Item : Target->Controls())
	{
		const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
		if (ItemContainer)
		{
			std::shared_ptr<Tree> Child = Root->AddChild(ItemContainer->GetType());
			PopulateTree(Child, ItemContainer);
		}
		else
		{
			Root->AddChild(Item->GetType())->SetMetaData(Item.get());
		}
	}
}

void Inspector::Populate()
{
	std::shared_ptr<Splitter> Split = m_Root.lock();
	std::shared_ptr<ScrollableViewControl> TreeView = Split->First()->AddControl<ScrollableViewControl>();
	std::shared_ptr<ScrollableViewControl> PropertiesView = Split->Second()->AddControl<ScrollableViewControl>();

	TreeView->SetExpand(Expand::Both);
	PropertiesView->SetExpand(Expand::Both);

	std::shared_ptr<Properties> Props = PropertiesView->Scrollable()->AddControl<Properties>();
	m_Properties = Props;

	std::shared_ptr<Tree> Root = TreeView->Scrollable()->AddControl<Tree>();
	Root
		->SetOnSelected([this](Tree& Item) -> void
			{
				Control const* MetaData = static_cast<Control const*>(Item.MetaData());
				ParseProperty(MetaData);
			})
		.SetExpand(Expand::Width);
	PopulateTree(Root, m_Target->GetRootContainer());
	m_Tree = Root;
}

bool Inspector::ExpandTree(const std::shared_ptr<Tree>& Root, Control const* Target)
{
	if (Target == nullptr || !Root)
	{
		return false;
	}

	if (Root->MetaData() == Target)
	{
		Root->SetSelected(true);
		Root->SetExpanded(true);
		return true;
	}
	else
	{
		for (const std::shared_ptr<Tree>& Item : Root->Children())
		{
			if (ExpandTree(Item, Target))
			{
				Root->SetExpanded(true);
				return true;
			}
		}
	}

	return false;
}

void Inspector::ParseProperty(Control const* Target)
{
	if (Target == nullptr || m_Properties.expired())
	{
		return;
	}

	Json Root(Json::Type::Object);
	Target->OnSave(Root);
	m_Properties.lock()->Parse(Root);
}

void Inspector::Close()
{
	m_Proxy->Clear();
	m_Target
		->SetOnClose(nullptr)
		->GetContainer()->RemoveControl(m_Proxy);
	m_Target = nullptr;
}

}
}
