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
#include "../Controls/Panel.h"
#include "../Controls/Text.h"
#include "../Controls/Tree.h"
#include "../Paint.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "Profiler.h"

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
		const std::vector<Profiler::Frame>& Frames = Profile.Frames();
		for (const Profiler::Frame& Frame_ : Frames)
		{
			m_MaxElapsed = std::max<int64_t>(m_MaxElapsed, Frame_.Elapsed());
		}
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

		const size_t StartColumn = (size_t)m_Offset / m_ColumnSize;

		float Offset = 0.0f;
		for (size_t I = StartColumn; I < Frames.size(); I++)
		{
			const Profiler::Frame& Frame_ = Frames[I];

			const float Pct = (float)Frame_.Elapsed() / (float)m_MaxElapsed;
			const float Height = GetSize().Y * Pct;
			const Vector2 Bottom = GetAbsolutePosition() + Vector2(0.0f, GetSize().Y);
			const Vector2 Min = Bottom + Vector2(Offset, -Height);
			const Vector2 Max = Bottom + Vector2(Offset + m_ColumnSize, 0.0f);
			Brush.Rectangle({ Min, Max }, m_HoveredIndex == I ? HoveredColor : BaseColor);
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
		}

		m_Anchor = Position;
	}

	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override
	{
		if (Button == Mouse::Button::Right)
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

	int64_t m_MaxElapsed { 75 };
	int m_HoveredIndex { -1 };
	float m_ColumnSize { 4.0f };
	bool m_Drag { false };
	float m_Offset { 0.0f };
	Vector2 m_Anchor {};

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
		std::shared_ptr<Panel> Background = AddControl<Panel>();
		Background
			->SetProperty(ThemeProperties::Panel, Color::White)
			.SetExpand(Expand::Both);

		m_Track = AddControl<TimelineTrack>();

		SetExpand(Expand::Width);
	}

	const std::shared_ptr<TimelineTrack>& Track() const
	{
		return m_Track;
	}

	virtual Vector2 DesiredSize() const override
	{
		return { 0.0f, 100.0f };
	}

	void Initialize(Profiler& Profile)
	{
		m_Track->Initialize(Profile);
	}

private:
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
			   << "\"Width\": 600,"
			   << "\"Height\": 300,"
			   << "\"Body\": {\"Controls\": ["
			   << "{\"Type\": \"Panel\", \"Expand\": \"Both\"},"
			   << "{\"Type\": \"MarginContainer\", \"Left\": 4, \"Top\": 4, \"Right\": 4, \"Bottom\": 4, \"Controls\": ["
			   << "{\"ID\": \"Root\", \"Type\": \"VerticalContainer\", \"Expand\": \"Both\", \"Controls\": ["
			   << "]}"
			   << "]}"
			   << "]}}";

		ControlList List;
		m_Window = InWindow->App().NewWindow("ProfileViewer", Stream.str().c_str(), List);
		std::shared_ptr<Container> Root = List.To<Container>("Root");
		m_Root = Root;

		std::string TotalFrames = "Frame Count: " + std::to_string(Profiler::Get().Frames().size());
		Root->AddControl<Text>()->SetText(TotalFrames.c_str());

		std::shared_ptr<Timeline> Timeline_ = Root->AddControl<Timeline>();
		Timeline_->Track()->SetOnHovered([this](int Index) -> void
			{
				SetFrame(Index);
			});
		m_Timeline = Timeline_;

		m_Tree = Root->AddControl<Tree>();
		m_Tree.lock()->SetExpand(Expand::Width);
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

void ProfileViewer::Populate()
{
	m_Timeline.lock()->Initialize(Profiler::Get());
	SetFrame(0);
}

void ProfileViewer::SetFrame(size_t Index)
{
	std::shared_ptr<Tree> Tree_ = m_Tree.lock();
	Tree_->Clear();

	const std::vector<Profiler::Frame>& Frames = Profiler::Get().Frames();
	if (Index < Frames.size())
	{
		const Profiler::Frame& Frame_ = Frames[Index];
		int64_t Elapsed = !Frame_.Events().empty() ? Frame_.Events().back().Elapsed() : 0;
		const std::string Label = "Frame [" + std::to_string(Index) + "]: " + std::to_string(Elapsed);
		Tree_->SetText(Label.c_str());

		const std::vector<Profiler::Event>& Events = Frame_.Events();
		for (size_t I = 0; !Events.empty() && I < Events.size() - 1; I++)
		{
			const Profiler::Event& Event_ = Events[I];
			Tree_->AddChild((std::string(Event_.Name()) + " " + std::to_string(Event_.Elapsed())).c_str());
		}
	}
}

}
}
