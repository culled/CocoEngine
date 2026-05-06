//
// Created by cullen on 2/28/26.
//

#ifndef COCOENGINE_WINDOW_H
#define COCOENGINE_WINDOW_H
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Types/Event.h>
#include <Coco/Core/Math/Size.h>
#include <Coco/Core/Math/Vector2.h>
#include <Coco/Core/Memory/Refs.h>
#include "WindowTypes.h"
#include "../Rendering/Graphics/Resources/GraphicsSurface.h"

namespace Coco
{
    class Window;

    /// @brief Parameters for creating a Window
    struct WindowCreateParams
    {
        /// @brief The title of the Window
        const char* Title;

        /// @brief The initial size of the Window
        Sizei InitialSize;

        /// @brief If true, the Window will start fullscreen (defaults to false)
        bool IsFullscreen;

        /// @brief If true, the Window can be resized by the user (defaults to true)
        bool CanResize;

        /// @brief If true, the Window will be focused when it is shown (defaults to true)
        bool FocusWhenShown;

        /// @brief Sets the initial state for the Window (defaults to WindowState::Default)
        WindowState InitialState;

        /// @brief Flags for the Window style (defaults to WindowStyleFlags::None)
        WindowStyleFlags StyleFlags;

        /// @brief If given, will set the initial position for the Window (the top-left point)
        Optional<Vector2i> InitialPosition;

        /// @brief If given, will set the display that the Window opens on
        Optional<uint16> InitialDisplayIndex;

        /// @brief Will set the given Window as the new Window's parent
        Ref<Window> ParentWindow;

        WindowCreateParams(const char* title, const Sizei& initialSize);
    };

    class Window
    {
    public:
        Event<const bool&> OnVisibilityChanged;
        Event<const WindowState&> OnStateChanged;
        Event<const Sizei&> OnSizeChanged;
        Event<const Vector2i&> OnPositionChanged;
        Event<const bool&> OnFocusChanged;
        Event<bool&> OnCloseRequested;
        Event<> OnClosed;

    public:
        virtual ~Window() = default;

        virtual void Show() = 0;
        virtual bool IsVisible() const = 0;
        virtual void SetTitle(const char* title) = 0;
        virtual String GetTitle() const = 0;
        virtual void SetSize(const Sizei& newSize) = 0;
        virtual Sizei GetSize() const = 0;
        virtual void SetPosition(const Vector2i& newPos, bool relativeToParent) = 0;
        virtual Vector2i GetPosition(bool relativeToParent) const = 0;
        virtual void SetState(WindowState newState) = 0;
        virtual WindowState GetState() const = 0;
        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual bool IsFullscreen() const = 0;
        virtual void Focus() = 0;
        virtual bool IsFocused() const = 0;

        uint32 GetID() const { return _id; }
        bool IsMainWindow() const { return _isMainWindow; }
        void RequestClose();
        Ref<GraphicsSurface> GetSurface() { return _surface; }

    protected:
        Window(uint32 id, bool isMainWindow, Ref<Window> parentWindow);

        virtual void Close() = 0;

    protected:
        uint32 _id;
        Ref<Window> _parent;
        bool _isMainWindow;
        Ref<GraphicsSurface> _surface;
    };
} // Coco

#endif //COCOENGINE_WINDOW_H