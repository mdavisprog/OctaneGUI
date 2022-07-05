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

#include "ProfileViewer.h"
#include "../Application.h"
#include "../Controls/Container.h"
#include "../Controls/ControlList.h"
#include "../Controls/HorizontalContainer.h"
#include "../Controls/MenuItem.h"
#include "../Controls/Panel.h"
#include "../Controls/ScrollableContainer.h"
#include "../Controls/ScrollableViewControl.h"
#include "../Controls/Table.h"
#include "../Controls/Text.h"
#include "../Controls/Tree.h"
#include "../Controls/VerticalContainer.h"
#include "../Paint.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "Profiler.h"

#include <cassert>
#include <sstream>

namespace OctaneGUI
{
namespace Tools
{

class TimelineTrack : public Control
{
	CLASS(TimelineTrack)

public:
	typedef std::function<void(int)> OnIndexSignature;

	TimelineTrack(Window* InWindow)
		: Control(InWindow)
	{
		SetExpand(Expand::Both);
	}

	void Initialize(Profiler& Profile)
	{
		Update(Profile);
		SetSelected(0);
	}

	void SetViewMode(ProfileViewer::ViewMode Mode)
	{
		if (m_ViewMode == Mode)
		{
			return;
		}

		m_ViewMode = Mode;
		Update(Profiler::Get());
	}

	TimelineTrack& SetOnSelected(OnIndexSignature&& Fn)
	{
		m_OnSelected = std::move(Fn);
		return *this;
	}

	TimelineTrack& SetOnHovered(OnIndexSignature&& Fn)
	{
		m_OnHovered = std::move(Fn);
		return *this;
	}

	virtual void OnPaint(Paint& Brush) const override
	{
		const std::vector<Profiler::Frame>& Frames = Profiler::Get().Frames();
		const Color BaseColor = GetProperty(ThemeProperties::Button).ToColor();
		const Color HoveredColor = GetProperty(ThemeProperties::Button_Hovered).ToColor();
		const Color SelectedColor = Color(255, 255, 0, 255);

		const size_t StartColumn = (size_t)m_Offset / m_ColumnSize;

		float Offset = 0.0f;
		for (size_t I = StartColumn; I < Frames.size(); I++)
		{
			Color EventColor = BaseColor;
			if (m_SelectedIndex == I)
			{
				EventColor = SelectedColor;
			}
			else if (m_HoveredIndex == I)
			{
				EventColor = HoveredColor;
			}

			const Profiler::Frame& Frame = Frames[I];

			int64_t Value = m_ViewMode == ProfileViewer::ViewMode::Elapsed ? Frame.Elapsed() : Frame.InclusiveCount();
			const float Pct = (float)Value / (float)m_MaxValue;
			const float Height = GetSize().Y * Pct;
			const Vector2 Bottom = GetAbsolutePosition() + Vector2(0.0f, GetSize().Y);
			const Vector2 Min = Bottom + Vector2(Offset, -Height);
			const Vector2 Max = Bottom + Vector2(Offset + m_ColumnSize, 0.0f);
			Brush.Rectangle({ Min, Max }, EventColor);
			Offset += m_ColumnSize;

			if (Offset >= GetSize().X)
			{
				break;
			}
		}

		Brush.RectangleOutline(GetAbsoluteBounds(), GetProperty(ThemeProperties::PanelOutline).ToColor());
	}

	virtual void OnMouseMove(const Vector2& Position) override
	{
		if (m_Drag)
		{
			const std::vector<Profiler::Frame>& Frames = Profiler::Get().Frames();

			const Vector2 Delta = Position - m_Anchor;
			const float TotalSize = (float)Frames.size() * m_ColumnSize;
			const float Overflow = TotalSize - GetSize().X;

			if (Overflow > 0.0f)
			{
				m_Offset = std::max<float>(0.0f, m_Offset - Delta.X);
				m_Offset = std::min<float>(m_Offset, Overflow);
				Invalidate();
			}
		}
		else if (Contains(Position))
		{
			m_HoveredIndex = GetIndex(Position);

			if (m_OnHovered)
			{
				m_OnHovered(m_HoveredIndex);
			}

			Invalidate();
		}

		m_Anchor = Position;
	}

	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override
	{
		if (Button == Mouse::Button::Left)
		{
			SetSelected(GetIndex(Position));
			Invalidate();
		}
		else if (Button == Mouse::Button::Right)
		{
			m_Drag = true;
		}

		return true;
	}

	virtual void OnMouseReleased(const Vector2& Position, Mouse::Button Button) override
	{
		if (Button == Mouse::Button::Right)
		{
			m_Drag = false;
		}
	}

private:
	int GetIndex(const Vector2& Position) const
	{
		const Vector2 Local = Position - GetAbsolutePosition();
		return (Local.X + m_Offset) / m_ColumnSize;
	}

	void SetSelected(int Index)
	{
		if (m_SelectedIndex == Index)
		{
			return;
		}

		m_SelectedIndex = Index;

		if (m_OnSelected)
		{
			m_OnSelected(m_SelectedIndex);
		}
	}

	void Update(Profiler& Profile)
	{
		m_MaxValue = 50;
		const std::vector<Profiler::Frame>& Frames = Profile.Frames();
		for (const Profiler::Frame& Frame : Frames)
		{
			if (m_ViewMode == ProfileViewer::ViewMode::Elapsed)
			{
				m_MaxValue = std::max<int64_t>(m_MaxValue, Frame.Elapsed());
			}
			else
			{
				m_MaxValue = std::max<int64_t>(m_MaxValue, Frame.InclusiveCount());
			}
		}
	}

	int64_t m_MaxValue { 50 };
	int m_HoveredIndex { -1 };
	int m_SelectedIndex { 0 };
	float m_ColumnSize { 4.0f };
	bool m_Drag { false };
	float m_Offset { 0.0f };
	Vector2 m_Anchor {};
	ProfileViewer::ViewMode m_ViewMode { ProfileViewer::ViewMode::Elapsed };

	OnIndexSignature m_OnSelected { nullptr };
	OnIndexSignature m_OnHovered { nullptr };
};

class Timeline : public Container
{
	CLASS(Timeline)

public:
	Timeline(Window* InWindow)
		: Container(InWindow)
	{
		m_Background = AddControl<Panel>();
		m_Background->SetExpand(Expand::Both);

		m_Track = AddControl<TimelineTrack>();

		SetExpand(Expand::Width);

		OnThemeLoaded();
	}

	void Initialize(Profiler& Profile)
	{
		m_Track->Initialize(Profile);
	}

	const std::shared_ptr<TimelineTrack>& Track() const
	{
		return m_Track;
	}

	virtual Vector2 DesiredSize() const override
	{
		return { 0.0f, 100.0f };
	}

	virtual void OnThemeLoaded() override
	{
		m_Background->SetProperty(ThemeProperties::Panel, GetProperty(ThemeProperties::TextInput_Background));
	}

private:
	std::shared_ptr<Panel> m_Background { nullptr };
	std::shared_ptr<TimelineTrack> m_Track { nullptr };
};

ProfileViewer& ProfileViewer::Get()
{
	static ProfileViewer s_ProfileViewer;
	return s_ProfileViewer;
}

void ProfileViewer::View(Window* InWindow)
{
	if (m_Window.expired())
	{
		std::stringstream Stream;
		Stream << "{\"Title\": \"ProfileViewer\","
			   << "\"Width\": 800,"
			   << "\"Height\": 400,"
			   << "\"MenuBar\": {\"Items\": ["
			   << "{\"ID\": \"View\", \"Text\": \"View\", \"Items\": ["
			   << "{\"ID\": \"Elapsed\", \"Text\": \"Elapsed Time\"}, {\"ID\": \"Count\", \"Text\": \"Event Count\"}"
			   << "]}]},"
			   << "\"Body\": {\"Controls\": ["
			   << "{\"Type\": \"Panel\", \"Expand\": \"Both\"},"
			   << "{\"Type\": \"MarginContainer\", \"Margins\": [4, 4, 4, 4], \"Expand\": \"Both\", \"Controls\": ["
			   << "{\"ID\": \"Root\", \"Type\": \"VerticalContainer\", \"Expand\": \"Both\", \"Controls\": ["
			   << "]}"
			   << "]}"
			   << "]}}";

		ControlList List;
		m_Window = InWindow->App().NewWindow("ProfileViewer", Stream.str().c_str(), List);

		std::shared_ptr<MenuItem> ViewElapsed = List.To<MenuItem>("View.Elapsed");
		std::shared_ptr<MenuItem> ViewCount = List.To<MenuItem>("View.Count");

		ViewElapsed->SetChecked(true)
			.SetOnPressed([this, ViewElapsed, ViewCount](const TextSelectable&) -> void
				{
					ViewElapsed->SetChecked(true);
					ViewCount->SetChecked(false);
					SetViewMode(ViewMode::Elapsed);
				});

		ViewCount->SetOnPressed([this, ViewElapsed, ViewCount](const TextSelectable&) -> void
			{
				ViewElapsed->SetChecked(false);
				ViewCount->SetChecked(true);
				SetViewMode(ViewMode::Count);
			});

		std::shared_ptr<Container> Root = List.To<Container>("Root");
		m_Root = Root;

		std::shared_ptr<HorizontalContainer> Info = Root->AddControl<HorizontalContainer>();
		Info->SetSpacing({ 20.0f, 0.0f });
		std::string TotalFrames = "Frame Count: " + std::to_string(Profiler::Get().Frames().size());
		Info->AddControl<Text>()->SetText(TotalFrames.c_str());
		m_HoveredFrame = Info->AddControl<Text>();

		std::shared_ptr<Timeline> Timeline_ = Root->AddControl<Timeline>();
		Timeline_->Track()
			->SetOnSelected([this](int Index) -> void
				{
					SetFrame(Index);
				})
			.SetOnHovered([this](int Index)
				{
					const std::vector<Profiler::Frame>& Frames = Profiler::Get().Frames();
					if (Index < Frames.size())
					{
						std::string Contents = std::string("Frame [") + std::to_string(Index) + "]: " + std::to_string(Frames[Index].Elapsed());
						Contents += " " + std::to_string(Frames[Index].InclusiveCount());
						m_HoveredFrame.lock()->SetText(Contents.c_str());
					}
				});
		m_Timeline = Timeline_;

		std::shared_ptr<ScrollableViewControl> FrameDescView = Root->AddControl<ScrollableViewControl>();
		FrameDescView->SetExpand(Expand::Both);

		std::shared_ptr<Table> FrameDesc = FrameDescView->Scrollable()->AddControl<Table>();
		FrameDesc
			->SetColumns(4)
			.SetHeader(0, "Frame")
			.SetHeader(1, "Time")
			.SetHeader(2, "Inclusive Count")
			.SetHeader(3, "Exclusive Count")
			.SetColumnSpacing(8.0f);

		m_Tree = FrameDesc->Column(0)->AddControl<Tree>();
		m_Tree.lock()
			->SetOnToggled([this](Tree& Root) -> void
				{
					UpdateFrameInfo();
				})
			.SetExpand(Expand::Width);

		const std::shared_ptr<BoxContainer>& FrameTimes = std::static_pointer_cast<BoxContainer>(FrameDesc->Column(1));
		FrameTimes->SetSpacing({ 0.0f, 0.0f });
		m_FrameTimes = FrameTimes;

		const std::shared_ptr<BoxContainer>& InclusiveEventCount = std::static_pointer_cast<BoxContainer>(FrameDesc->Column(2));
		InclusiveEventCount->SetSpacing({ 0.0f, 0.0f });
		m_InclusiveEventCount = InclusiveEventCount;

		const std::shared_ptr<BoxContainer>& ExclusiveEventCount = std::static_pointer_cast<BoxContainer>(FrameDesc->Column(3));
		ExclusiveEventCount->SetSpacing({ 0.0f, 0.0f });
		m_ExclusiveEventCount = ExclusiveEventCount;
	}

	if (m_Window.lock()->IsVisible())
	{
		return;
	}

	InWindow->App().DisplayWindow("ProfileViewer");
	Populate();
}

ProfileViewer::ProfileViewer()
{
}

void ProfileViewer::SetViewMode(ViewMode Mode)
{
	m_Timeline.lock()->Track()->SetViewMode(Mode);
}

void ProfileViewer::Populate()
{
	m_Timeline.lock()->Initialize(Profiler::Get());
	SetFrame(0);
}

void AddEvents(const std::shared_ptr<Tree>& Root, const Profiler::Event& Event)
{
	for (const Profiler::Event& Item : Event.Events())
	{
		std::shared_ptr<Tree> Child = Root->AddChild(Item.Name());

		if (!Item.Events().empty())
		{
			AddEvents(Child, Item);
		}
	}
}

const Profiler::Frame& GetFrame(size_t Index)
{
	assert(Index >= 0 && Index < Profiler::Get().Frames().size());
	return Profiler::Get().Frames()[Index];
}

void ProfileViewer::SetFrame(size_t Index)
{
	std::shared_ptr<Tree> Tree_ = m_Tree.lock();
	bool Expanded = Tree_->IsExpanded();
	Tree_->Clear();

	const std::vector<Profiler::Frame>& Frames = Profiler::Get().Frames();
	if (Index < Frames.size())
	{
		m_FrameIndex = Index;
		const Profiler::Frame& Frame = Frames[Index];
		const std::string Label = "Frame [" + std::to_string(Index) + "]";
		Tree_->SetText(Label.c_str());

		const std::vector<Profiler::Event>& Events = Frame.Events();
		for (const Profiler::Event& Event : Events)
		{
			std::shared_ptr<Tree> Child = Tree_->AddChild(Event.Name());
			if (!Event.Events().empty())
			{
				AddEvents(Child, Event);
			}
		}
	}

	Tree_->SetExpanded(Expanded);
	UpdateFrameInfo();
}

void AddRow(const std::shared_ptr<Container>& Column, int64_t Value)
{
	std::shared_ptr<HorizontalContainer> Outer = Column->AddControl<HorizontalContainer>();
	Outer
		->SetGrow(Grow::Center)
		->SetExpand(Expand::Width);
	Outer->AddControl<Text>()->SetText(std::to_string(Value).c_str());
}

void UpdateRow(const Tree& Root, const Profiler::Event& Event, const std::shared_ptr<Container>& FrameTimes, const std::shared_ptr<Container>& Inclusive, const std::shared_ptr<Container>& Exclusive)
{
	AddRow(FrameTimes, Event.Elapsed());
	AddRow(Inclusive, Event.InclusiveCount());
	AddRow(Exclusive, Event.ExclusiveCount());

	if (!Root.IsExpanded())
	{
		return;
	}

	int Index = 0;
	Root.ForEachChild([&](const Tree& Child) -> void
		{
			UpdateRow(Child, Event.Events()[Index], FrameTimes, Inclusive, Exclusive);
			Index++;
		});
}

void ProfileViewer::UpdateFrameInfo()
{
	std::shared_ptr<Tree> FrameTree = m_Tree.lock();
	std::shared_ptr<Container> FrameTimes = m_FrameTimes.lock();
	std::shared_ptr<Container> Inclusive = m_InclusiveEventCount.lock();
	std::shared_ptr<Container> Exclusive = m_ExclusiveEventCount.lock();

	const Profiler::Frame& Frame = GetFrame(m_FrameIndex);

	FrameTimes->ClearControls();
	Inclusive->ClearControls();
	Exclusive->ClearControls();

	AddRow(FrameTimes, Frame.Elapsed());
	AddRow(Inclusive, Frame.InclusiveCount());
	AddRow(Exclusive, Frame.ExclusiveCount());

	if (!FrameTree->IsExpanded())
	{
		return;
	}

	int Index = 0;
	FrameTree->ForEachChild([this, &Frame, &Index](const Tree& Child) -> void
		{
			UpdateRow(Child, Frame.Events()[Index], m_FrameTimes.lock(), m_InclusiveEventCount.lock(), m_ExclusiveEventCount.lock());
			Index++;
		});
}

}
}
