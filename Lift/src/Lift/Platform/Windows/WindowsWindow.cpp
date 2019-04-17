#include "pch.h"

#include "WindowsWindow.h"

#include "Lift/Events/ApplicationEvent.h"
#include "Lift/Events/MouseEvent.h"
#include "Lift/Events/KeyEvent.h"

#include <glad/glad.h>

namespace Lift {
	
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description) {
		LF_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
		
	}
	Window* Window::Create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		Init(props);
	}

	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}

	void WindowsWindow::OnUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_windowHandle);
	}

	unsigned WindowsWindow::GetWidth() const {
		return  m_data.width;
	}

	unsigned WindowsWindow::GetHeight() const {
		return m_data.height;
	}

	void WindowsWindow::SetEventCallback(const EventCallbackFn& callback) {
		m_data.eventCallback = callback;
	}

	void WindowsWindow::SetVSync(bool enabled) {
		if(enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_data.vSync = enabled;
	}

	bool WindowsWindow::IsVSync() const {
		return  m_data.vSync;
	}

	void* WindowsWindow::GetNativeWindow() const {
		return m_windowHandle;
	}

	void WindowsWindow::Init(const WindowProps& props) {
		m_data.title = props.Title;
		m_data.width = props.Width;
		m_data.height = props.Height;

		LF_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized) {
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			LF_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		m_windowHandle = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_windowHandle);
		const int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		LF_CORE_ASSERT(status, "Failed to initialize Glad!");
		glfwSetWindowUserPointer(m_windowHandle, &m_data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_windowHandle, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(m_windowHandle, [](GLFWwindow* window) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			WindowCloseEvent event;
			data.eventCallback(event);
		});

		glfwSetKeyCallback(m_windowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(key, 0);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent event(key, 1);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_windowHandle, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			KeyTypedEvent event(keycode);
			data.eventCallback(event);
		});

		glfwSetMouseButtonCallback(m_windowHandle, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_windowHandle, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(m_windowHandle, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
			data.eventCallback(event);
});
	}

	void WindowsWindow::Shutdown() {
		glfwDestroyWindow(m_windowHandle);
	}

	
}
