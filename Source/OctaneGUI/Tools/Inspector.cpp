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
#include "../Controls/Tree.h"
#include "../Window.h"

#include <sstream>

namespace OctaneGUI
{
namespace Tools
{

Inspector& Inspector::Get()
{
	return s_Inspector;
}

void Inspector::Inspect(const std::shared_ptr<Container>& Target)
{
	if (m_Window.expired())
	{
		std::stringstream Stream;
		Stream << "{\"Title\": \"Inspecting "
			   << Target->GetWindow()->GetTitle()
			   << "\","
			   << "\"Width\": 300,"
			   << "\"Height\": 200,"
			   << "\"Body\": {\"Controls\": ["
			   << "{\"ID\": \"Scrollable\", \"Type\": \"ScrollableContainer\"}"
			   << "]}}";

		ControlList List;
		m_Window = Target->GetWindow()->App().NewWindow("Inspector", Stream.str().c_str(), List);
		m_Root = List.To<Container>("Scrollable");
	}

	if (m_Window.lock()->IsVisible())
	{
		return;
	}

	m_Target = Target;
	Target->GetWindow()->App().DisplayWindow("Inspector");
	m_Root.lock()->ClearControls();
	Populate();
}

Inspector Inspector::s_Inspector {};

Inspector::Inspector()
{
}

static void PopulateTree(const std::shared_ptr<Tree>& Root, const std::shared_ptr<Container>& Target)
{
	Root->SetText(Target->GetType());

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
			Root->AddChild(Item->GetType());
		}
	}
}

void Inspector::Populate()
{
	std::shared_ptr<Tree> Root = m_Root.lock()->AddControl<Tree>();
	Root->SetExpand(Expand::Width);
	PopulateTree(Root, m_Target.lock());
}

}
}
