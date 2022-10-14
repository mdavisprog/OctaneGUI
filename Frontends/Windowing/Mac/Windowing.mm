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

#include "../Windowing.h"
#include "OctaneGUI/OctaneGUI.h"
#include "Windowing.h"

#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

namespace Frontend
{
namespace Windowing
{

namespace Mac
{

void SetMovable(void* Handle, bool Movable)
{
	NSWindow* Window = (NSWindow*)Handle;
	Window.movable = Movable ? YES : NO;
}

void HideTitleBar(void* Handle)
{
	NSWindow* Window = (NSWindow*)Handle;
	Window.titleVisibility = NSWindowTitleHidden;
	Window.titlebarAppearsTransparent = YES;
	Window.movableByWindowBackground = YES;
	Window.styleMask |= NSWindowStyleMaskFullSizeContentView;

	[[Window standardWindowButton:NSWindowCloseButton] setHidden:YES];
	[[Window standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
	[[Window standardWindowButton:NSWindowZoomButton] setHidden:YES];
}

void ShowMinimize(void* Handle, bool Show)
{
	NSWindow* Window = (NSWindow*)Handle;
	NSButton* Minimize = [Window standardWindowButton:NSWindowMiniaturizeButton];
	[Minimize setHidden:Show ? NO : YES];
}

std::u32string FileDialog(OctaneGUI::FileDialogType Type, const std::vector<OctaneGUI::FileDialogFilter>& Filters, void* Handle)
{
	NSString* Title = Type == OctaneGUI::FileDialogType::Open
		? @"Open File" : @"Save File";
	
	NSString* Message = Type == OctaneGUI::FileDialogType::Open
		? @"Open a file" : @"Save to file";

	NSMutableArray* FileTypes = [NSMutableArray arrayWithCapacity:0];
	for (const OctaneGUI::FileDialogFilter& Filter : Filters)
	{
		for (const std::u32string& Extension : Filter.Extensions)
		{
			NSString* Ext = [NSString stringWithUTF8String:OctaneGUI::String::ToMultiByte(Extension).c_str()];
			UTType* Type = [UTType typeWithFilenameExtension:Ext];
			[FileTypes addObject:Type];
		}
	}

	NSWindow* KeyWindow = [NSApp keyWindow];
	NSSavePanel* Dialog = [NSSavePanel savePanel];
	if (Type == OctaneGUI::FileDialogType::Open)
	{
		NSOpenPanel* OpenDialog = [NSOpenPanel openPanel];
		[OpenDialog setCanChooseFiles:true];
		[OpenDialog setAllowsMultipleSelection:FALSE];
		[OpenDialog setCanChooseDirectories:false];
		Dialog = OpenDialog;
	}
	[Dialog setLevel:CGShieldingWindowLevel()];
	[Dialog setTitle:Title];
	[Dialog setMessage:Message];
	[Dialog setCanCreateDirectories:TRUE];
	[Dialog setAllowedContentTypes:FileTypes];

	std::string Result;
	if ([Dialog runModal] == NSModalResponseOK)
	{
		NSURL* URL = [Dialog URL];
		NSString* Path = [[URL path] precomposedStringWithCanonicalMapping];
		Result = [Path UTF8String];
	}

	[KeyWindow makeKeyWindow];

	return OctaneGUI::String::ToUTF32(Result);
}

bool IsZoomed(void* Handle)
{
	NSWindow* Window = (NSWindow*)Handle;
	return Window.zoomed;
}

void Zoom(void* Handle)
{
	NSWindow* Window = (NSWindow*)Handle;
	[Window zoom:nil];
}

}

}
}
