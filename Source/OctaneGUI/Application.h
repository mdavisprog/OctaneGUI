/**

MIT License

Copyright (c) 2022-2024 Mitchell Davis <mdavisprog@gmail.com>

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

#include "CallbackDefs.h"
#include "CommandLine.h"
#include "FileSystem.h"
#include "Keyboard.h"
#include "LanguageServer.h"
#include "Mouse.h"
#include "Network.h"
#include "SystemInfo.h"
#include "TextureCache.h"
#include "Vector2.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace OctaneGUI
{

class ControlList;
class Event;
class Icons;
class Json;
class Theme;
class VertexBuffer;
class Window;

namespace Tools
{
class Interface;
}

/// @enum WindowAction
/// @brief List of window actions that must be handled by the frontend.
enum class WindowAction : uint8_t
{
    Create,   ///< Create a window.
    Destroy,  ///< Destroy a window.
    Raise,    ///< Focus a window.
    Enable,   ///< Enables a window.
    Disable,  ///< Disables a window.
    Position, ///< Update the position of a window.
    Size,     ///< Update the size of a window.
    Minimize, ///< Minimize a window.
    Maximize, ///< Maximize a window.
};

///
/// The Application class encapsulates the GUI controls used for all windows of an
/// application. Callbacks must also be provided to handle the various events that
/// occur within the application that allow for the implementer to manage
/// rendering and event handling while the library manages the GUI controls and
/// their states.
///
/// There should only be a single instance of the Application class for any applications
/// using OctaneGUI. Once all callbacks are registered and Initialized has been called,
/// the Run function must be called to begin the GUI event loop.
///
class Application
{
public:
    typedef std::function<void(Window*)> OnWindowSignature;
    typedef std::function<void(Window*, const VertexBuffer&)> OnWindowPaintSignature;
    typedef std::function<Event(Window*)> OnWindowEventSignature;
    typedef std::function<void(Window*, WindowAction)> OnWindowActionSignature;
    typedef std::function<uint32_t(const std::vector<uint8_t>&, uint32_t, uint32_t)> OnLoadTextureSignature;
    typedef std::function<void(const std::u32string&)> OnSetClipboardContentsSignature;
    typedef std::function<std::u32string(void)> OnGetClipboardContentsSignature;
    typedef std::function<void(Window*, const char32_t*)> OnSetWindowTitleSignature;
    typedef std::function<void(Window*, Mouse::Cursor)> OnSetMouseCursorSignature;
    typedef std::function<void(Window*, const Vector2&)> OnSetMousePositionSignature;
    typedef std::function<std::shared_ptr<Control>(Container*, const std::string&)> OnCreateControlSignature;

    Application();
    virtual ~Application();

    /// @brief Initializes the application with the given JSON stream.
    ///
    /// This is the first function that needs to be called before anything else in order to
    /// use the Application object. The JSON stream given must include certain properties:
    ///
    /// * Theme: This should be the name of the file that contains a list of definitions
    /// to define the look of the application.
    /// * Icons: A JSON object that defines the icons to be used.
    /// * Windows: List of windows to create for this application. A 'Main' window must always
    /// be defined.
    /// * HighDPI: A boolean flag to determine if the implementing frontend should support high DPI
    /// awareness.
    /// * CustomTitleBar: Forces all windows to be created with no system title bar.
    /// * UseSystemFileDialog: Notifies the FileSystem object to use the system's file dialog instead
    /// of the custom one using the libraries controls.
    /// * UseNetwork: Will initialize the networking subsystem.
    ///
    /// The second parameter is a reference to a map<string, ControlList>, which returns all controls
    /// that has a 'ID' property in their JSON for their respective window defined by a string.
    ///
    /// @param JsonStream A JSON string object with the above properties defined.
    /// @param WindowControls List of controls that has a defined 'ID' property within their JSON
    /// description for any defined window.
    /// @return True if initialized successfully. False otherwise.
    bool Initialize(const char* JsonStream, std::unordered_map<std::string, ControlList>& WindowControls);

    /// @brief Destroy all existing windows and cleans up any used resources.
    ///
    /// This function can be called manually, but is also called automatically when the
    /// @ref Run "Run" function has ended.
    void Shutdown();

    /// @brief Updates the state of all windows.
    ///
    /// The state of each window is updated with each call to Update. The windows keep tracks of all
    /// controls that need to be updated. Most of the time, no updates will occur if no controls need
    /// to be layed out.
    ///
    /// This will also perform any re-painting of invalidated windows.
    void Update();

    /// @brief Runs the application within a loop until the 'Main' window is requested to close.
    ///
    /// This function will run every loop iteration as a frame and will sleep if no updates occurred
    /// within the frame. This is a blocking function and the object will be cleaned up when this returns.
    /// @return Exit code for the function. 0 is success.
    int Run();

    /// @brief Runs a single frame for the application.
    ///
    /// When a single frame is run, all events for all active windows are first processed. Then,
    /// each window is updated with the new state.
    ///
    /// @return Number of events processed this frame,
    int RunFrame();

    /// @brief Returns whether the app is still running.
    ///
    /// This will return false if the Application object was not initialized, or if there was
    /// a Quit request.
    ///
    /// @return True if the Application is still running. False otherwise.
    bool IsRunning() const;

    /// @brief Forces the application to break out of the Run loop.
    void Quit();

    /// @brief Sets the program's command-line variables. This should be called before Initialize.
    /// @param Argc Number of arguments.
    /// @param Argv Argument list.
    /// @return The Application object for chaining methods.
    Application& SetCommandLine(int Argc, char** Argv);

    /// @brief Retrieves the window with the 'Main' ID.
    /// @return std::shared_ptr<Window> object.
    std::shared_ptr<Window> GetMainWindow() const;

    /// @brief Retrieves the window with the given ID.
    /// @param ID String representing the window ID.
    /// @return std::shared_ptr<Window> object.
    std::shared_ptr<Window> GetWindow(const char* ID) const;

    /// @cond !IGNORE_FUNCTIONS
    /// @brief Used internally.
    std::shared_ptr<Window> EventFocus() const;
    /// @endcond

    /// @brief Determines if the given Window object is the Main window.
    /// @param InWindow Pointer to a Window object.
    /// @return True if the given window is the Main window. False otherwise.
    bool IsMainWindow(Window* InWindow) const;

    /// @brief Determines if a Window with the given ID exists in the Application.
    ///
    /// To clarify, this does not mean the Window has a frontend representation i.e.
    /// a system window. Just that a Window object and all of its contents exists.
    ///
    /// @param ID String representing the window ID.
    /// @return True if the Window object exists. False otherwise.
    bool HasWindow(const char* ID) const;

    /// @brief Creates a new Window object with the given ID and a JSON stream defining its contents.
    ///
    /// This function will only create the window object, but will not display the window. Use the
    /// DisplayWindow function to notify the frontend to create a system window.
    ///
    /// @param ID String representing the Window object.
    /// @param JsonStream JSON object defining the controls for the given window. Refer to the
    /// Window documentation on the details of the JSON object.
    /// @return std::shared_ptr<Window> object.
    std::shared_ptr<Window> NewWindow(const char* ID, const char* JsonStream);

    /// @brief Same as the NewWindow function, but accepts a ControlList parameter to retrieve any
    /// controls that have defined ID properties.
    /// @param ID String representing the Window object.
    /// @param JsonStream JSON object defining the controls for the given window. Refer to the
    /// Window documentation on the details of the JSON object.
    /// @param List List of controls that have ID properties defined for them.
    /// @return std::shared_ptr<Window> object.
    std::shared_ptr<Window> NewWindow(const char* ID, const char* JsonStream, ControlList& List);

    /// @brief Notifies the frontend to create a system window from the Window object with the
    /// given ID.
    /// @param ID String representing the Window object.
    /// @return True if displayed by frontend or already displayed. False otherwise.
    bool DisplayWindow(const char* ID);

    /// @brief Closes the system window with the ID of a Window object.
    /// @param ID String representing the Window object.
    void CloseWindow(const char* ID);

    /// @brief Retrieve the Application's Theme object. There is only one per application.
    /// @return std::shared_ptr<Theme> object.
    std::shared_ptr<Theme> GetTheme() const;

    /// @brief Retrieves the Application's Icons object. There is only one per application.
    /// @return std::shared_ptr<Icons> object.
    std::shared_ptr<Icons> GetIcons() const;

    /// @brief Retrieves the Application's TextureCache object. There is only one per application.
    /// @return TextureCache reference.
    TextureCache& GetTextureCache();

    /// @cond !IGNORE_FUNCTIONS
    /// @brief Used internally.
    bool IsKeyPressed(Keyboard::Key Key) const;
    Application& KeyPressed(Keyboard::Key Key);
    Application& KeyReleased(Keyboard::Key Key);
    Application& ClearKeys();
    /// @endcond

    /// @brief Places the given content to the system's clipboard.
    ///
    /// This function will invoke the callback given to SetOnSetClipboardContents which
    /// should be implemented by the frontend.
    ///
    /// @param Contents String to add to the clipboard.
    void SetClipboardContents(const std::u32string& Contents);

    /// @brief Retrieves the contents of the system's clipboard.
    ///
    /// This function will invoke the callback given to SetOnGetClipboardContents which
    /// should be implemented by the frontend.
    ///
    /// @return String value of the system's clipboard contents.
    std::u32string ClipboardContents() const;

    /// @brief Sets the system's mouse cursor.
    ///
    /// This function will invoke the callback given to SetOnSetMouseCursur which should
    /// be implemented by the frontend.
    ///
    /// @param Target The Window object requesting the change.
    /// @param Cursor The mouse cursor type defined by the Cursor enum.
    /// @return The Application object for chaining.
    Application& SetMouseCursor(Window* Target, Mouse::Cursor Cursor);

    /// @brief Sets the system's mouse position.
    /// @param Target The Window object to set the mouse position relative to. This can be NULL to set
    /// an absolute position.
    /// @param Position The position of the mouse. Can be relative to the Target window or absolute.
    /// @return The Application object for chaining.
    Application& SetMousePosition(Window* Target, const Vector2& Position);

    /// @brief Const version of the FileSystem object.
    /// @return const FileSystem reference.
    const FileSystem& FS() const;

    /// @brief Mutable version of the FileSystem object.
    /// @return FileSystem reference.
    FileSystem& FS();

    /// @brief Const version of the LanguageServer object.
    /// @return const LanguageServer reference.
    const LanguageServer& LS() const;

    /// @brief Mutable version of the LanguageServer object.
    /// @return LanguageServer reference.
    LanguageServer& LS();

    /// @brief Const version of the Network object.
    /// @return const Network reference.
    const Network& Net() const;

    /// @brief Mutable version of the Network object.
    /// @return Network reference.
    Network& Net();

    /// @brief Attempt to create a control for the given Owner if a control of the given
    /// Type is not found.
    /// @param Owner The owning container to create the control for. The 'AddControl' function
    /// must be called.
    /// @param Type The type of the control to create.
    /// @return The newly created control within the 'Owner' container.
    std::shared_ptr<Control> CreateControl(Container* Owner, const std::string& Type) const;

    /// @brief Const version of the SystemInfo object.
    /// @return Const SystemInfo reference.
    const SystemInfo& GetSystemInfo() const;

    /// @brief Mutable version of the SystemInfo object.
    /// @return SystemInfo reference.
    SystemInfo& GetSystemInfo();

    /// @cond !IGNORE_FUNCTIONS
    /// @brief Used internally.
    const std::shared_ptr<Tools::Interface>& Tools();
    Application& SetIgnoreModals(bool IgnoreModals);
    /// @endcond

    /// @brief Window event notifications that must be handled by the frontend.
    ///
    /// This callback is invoked whenever a window requests an action to
    /// occur in the frontend, such as window creation, destruction, reposition,
    /// etc. Refer to the @ref OctaneGUI::WindowAction "WindowAction" enum for a list of actions that
    /// may be requested.
    ///
    /// @param Fn The OnWindowActionSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnWindowAction(OnWindowActionSignature&& Fn);

    /// @brief Window paint notification that must be handled by the frontend.
    ///
    /// This callback is invoked whenever a Window object needs to be repainted
    /// due to any updates that may have occurred within the window.
    ///
    /// @param Fn The OnWindowPaintSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnPaint(OnWindowPaintSignature&& Fn);

    /// @brief Notification for when a new frame begins.
    ///
    /// A new frame occurs during a Run loop whenever a new iteration begins and
    /// before any updates to Window objects are applied.
    ///
    /// @param Fn An OnEmptySignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnNewFrame(OnEmptySignature&& Fn);

    /// @brief Request for a system event from the frontend.
    ///
    /// This callback is invoked during the Run loop and for each Window. This
    /// callback will continued to be invoked for a Window until an Event type of
    /// None is returned.
    ///
    /// @param Fn The OnWindowEventSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnEvent(OnWindowEventSignature&& Fn);

    /// @brief Request for the frontend to load a texture.
    ///
    /// This callback is invoked whenever the library makes a request to load
    /// a texture.
    ///
    /// @param Fn The OnLoadTextureSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnLoadTexture(OnLoadTextureSignature&& Fn);

    /// @brief Callback invoked when the application is exiting.
    ///
    /// This is a good time for the frontend to cleanup any allocated
    /// resources.
    ///
    /// @param Fn An OnEmptySignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnExit(OnEmptySignature&& Fn);

    /// @brief Callback invoked when the application requests to set the system's
    /// clipboard contents.
    /// @param Fn The OnSetClipboardContentsSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnSetClipboardContents(OnSetClipboardContentsSignature&& Fn);

    /// @brief Callback invoked when the application requests to get the system's
    /// clipboard contents.
    /// @param Fn The OnGetClipboardContentsSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnGetClipboardContents(OnGetClipboardContentsSignature&& Fn);

    /// @brief Request to change the title of a system window.
    ///
    /// Whenever a Window object's title has changed, this callback will be invoked
    /// to notify the frontend to change the system window's title.
    ///
    /// @param Fn The OnSetWindowTitleSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnSetWindowTitle(OnSetWindowTitleSignature&& Fn);

    /// @brief Request to change the system's mouse cursor.
    /// @param Fn The OnSetMouseCursorSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnSetMouseCursor(OnSetMouseCursorSignature&& Fn);

    /// @brief Request to set the system's mouse position.
    /// @param Fn The OnSetMousePositionSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnSetMousePosition(OnSetMousePositionSignature&& Fn);

    /// @brief Callback invoked when a Container wishes to create a control that
    /// is not registered with OctaneGUI.
    /// @param Fn The OnCreateControlSignature callback.
    /// @return The Application object to allow for chaining methods.
    Application& SetOnCreateControl(OnCreateControlSignature&& Fn);

private:
    void OnPaint(Window* InWindow, const VertexBuffer& Buffer);
    std::shared_ptr<Window> CreateWindow(const char* ID);
    void DestroyWindow(const std::shared_ptr<Window>& Item);
    int ProcessEvent(const std::shared_ptr<Window>& Item);
    bool Initialize();
    void OnWindowAction(Window* InWindow, WindowAction Action);
    void LoadIcons(const Json& Root);
    void FocusWindow(const std::shared_ptr<Window>& Focus);
    std::shared_ptr<Window> FocusedWindow() const;

    CommandLine m_CommandLine {};
    std::unordered_map<std::string, std::shared_ptr<Window>> m_Windows;
    std::vector<std::weak_ptr<Window>> m_Modals {};
    std::weak_ptr<Window> m_EventFocus {};
    std::shared_ptr<Theme> m_Theme { nullptr };
    std::shared_ptr<Icons> m_Icons { nullptr };
    bool m_IsRunning { false };
    std::vector<Keyboard::Key> m_PressedKeys {};
    TextureCache m_TextureCache {};
    FileSystem m_FileSystem { *this };
    bool m_HighDPI { true };
    bool m_CustomTitleBar { false };
    LanguageServer m_LanguageServer {};
    Network m_Network {};
    // Moving these members to be outside of the TOOLS declaration to prevent different class layouts.
    // These members cannot be accessed with TOOLS disabled.
    bool m_IgnoreModals { false };
    std::shared_ptr<Tools::Interface> m_Tools { nullptr };
    SystemInfo m_SystemInfo {};

    OnWindowActionSignature m_OnWindowAction { nullptr };
    OnWindowPaintSignature m_OnPaint { nullptr };
    OnEmptySignature m_OnNewFrame { nullptr };
    OnWindowEventSignature m_OnEvent { nullptr };
    OnLoadTextureSignature m_OnLoadTexture { nullptr };
    OnEmptySignature m_OnExit { nullptr };
    OnSetClipboardContentsSignature m_OnSetClipboardContents { nullptr };
    OnGetClipboardContentsSignature m_OnGetClipboardContents { nullptr };
    OnSetWindowTitleSignature m_OnSetWindowTitle { nullptr };
    OnSetMouseCursorSignature m_OnSetMouseCursor { nullptr };
    OnSetMousePositionSignature m_OnSetMousePosition { nullptr };
    OnCreateControlSignature m_OnCreateControl { nullptr };
};

}
