#include "Window.hpp"

#include "../DaxaCore.hpp"

#include <iostream>

#include <VkBootstrap.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

namespace daxa {
	Window::Window(std::string name, std::array<u32, 2> size) 
		: name{ name }
		, size{ size }
	{
		//create blank SDL window for our application
		sdlWindowHandle = SDL_CreateWindow(
			name.c_str(),				// window title
			SDL_WINDOWPOS_UNDEFINED,	// window position x (don't care)
			SDL_WINDOWPOS_UNDEFINED,	// window position y (don't care)
			size[0],					// window width in pixels
			size[1],					// window height in pixels
			SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
		);

		SDL_CaptureMouse(SDL_TRUE);

		if (!sdlWindowHandle) {
			std::cerr << SDL_GetError() << std::endl;
			exit(-1);
		}

		sdlWindowId = SDL_GetWindowID(sdlWindowHandle);

		auto ret = SDL_Vulkan_CreateSurface((SDL_Window*)sdlWindowHandle, gpu::instance->getVkInstance(), &surface);
		DAXA_ASSERT_M(ret == SDL_TRUE, "could not create window surface");
	}
	Window::~Window() 	{
		vkDestroySurfaceKHR(gpu::instance->getVkInstance(), surface, nullptr);
		SDL_DestroyWindow(sdlWindowHandle);
		sdlWindowHandle = nullptr;
	}
	void Window::setSize(std::array<u32, 2> size) {
		bChangedSize = true;
		this->size = size;
	}
	std::array<u32, 2> Window::getSize() const 	{
		return size;
	}
	Vec2 Window::getSizeVec() const 	{
		return { static_cast<f32>(size[0]), static_cast<f32>(size[1]) };
	}
	void Window::setName(std::string name) 	{
		this->name = std::move(name);
	}
	const std::string& Window::getName() 	{
		return name;
	}
	bool Window::isFocused() const {
		return false;
	}
	bool Window::keyPressed(Scancode key) const {
		return !keyHidden[u32(key)] && (*keyStates)[u32(key)];
	}
	bool Window::keyJustPressed(Scancode key) const {
		return !keyHidden[u32(key)] && (*keyStates)[u32(key)] && !(*prevKeyStates)[u32(key)];
	}
	bool Window::keyReleased(Scancode key) const {
		return !keyHidden[u32(key)] && !(*keyStates)[u32(key)];
	}
	bool Window::keyJustReleased(Scancode key) const {
		return !keyHidden[u32(key)] && !(*keyStates)[u32(key)] && (*prevKeyStates)[u32(key)];
	}
	void Window::hideKey(Scancode key) {
		keyHidden[u32(key)] = true;
	}
	bool Window::buttonPressed(MouseButton button) const {
		return (*buttonStates)[u8(button)] && !buttonHidden[u8(button)];
	}
	bool Window::buttonJustPressed(MouseButton button) const {
		return !(*prevButtonStates)[u8(button)] && (*buttonStates)[u8(button)] && !buttonHidden[u8(button)];
	}
	bool Window::buttonReleased(MouseButton button) const {
		return !(*buttonStates)[u8(button)] && !buttonHidden[u8(button)];
	}
	bool Window::buttonJustReleased(MouseButton button) const {
		return (*prevButtonStates)[u8(button)] && !(*buttonStates)[u8(button)] && !buttonHidden[u8(button)];
	}
	void Window::hideButton(MouseButton button) {
		buttonHidden[u8(button)] = true;
	}
	bool Window::buttonPressedAndHide(MouseButton button) {
		auto ret = buttonPressed(button);
		hideButton(button);
		return ret;
	}
	bool Window::buttonJustPressedAndHide(MouseButton button) {
		auto ret = buttonJustPressed(button);
		hideButton(button);
		return ret;
	}
	bool Window::buttonReleasedAndHide(MouseButton button) {
		auto ret = buttonReleased(button);
		hideButton(button);
		return ret;
	}
	bool Window::buttonJustReleasedAndHide(MouseButton button) {
		auto ret = buttonJustReleased(button);
		hideButton(button);
		return ret;
	}
	std::array<i32, 2> Window::getCursorPosition() const {
		return cursorPos;
	}
	std::array<i32, 2> Window::getPrevCursorPosition() const {
		return prevCursorPos;
	}
	Vec2 Window::getCursorPositionVec() const {
		auto [x, y] = getCursorPosition();
		return Vec2{ static_cast<f32>(x), static_cast<f32>(y) };
	}
	Vec2 Window::getPrevCursorPositionVec() const {
		auto [x, y] = getPrevCursorPosition();
		return Vec2{ static_cast<f32>(x), static_cast<f32>(y) };
	}
	Vec2 Window::getCursorPositionRelative() const {
		return {
			f32(cursorPos[0]) / f32(size[0]) * 2.0f - 1.0f,
			f32(cursorPos[1]) / f32(size[1]) * 2.0f - 1.0f,
		};
	}
	std::array<i32, 2> Window::getCursorPositionChange() const {
		return {
			cursorPos[0] - prevCursorPos[0],
			cursorPos[1] - prevCursorPos[1],
		};
	}
	Vec2 Window::getCursorPositionChangeVec() const {
		auto [x, y] = getCursorPositionChange();
		return Vec2{ static_cast<f32>(x), static_cast<f32>(y) };
	}
	Vec2 Window::getCursorPositionChangeRelative() const {
		return getCursorPositionRelative() - Vec2{
			f32(prevCursorPos[0]) / f32(size[0]) * 2.0f - 1.0f,
			f32(prevCursorPos[1]) / f32(size[1]) * 2.0f - 1.0f,
		};
	}
	void Window::captureCursor() {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		bCursorCaptured = true;
	}
	void Window::releaseCursor() {
		SDL_SetRelativeMouseMode(SDL_FALSE);
		bCursorCaptured = false;
	}
	f32 Window::scrollX() const {
		return scrollXHidden ? 0.0f : m_scrollX;
	}
	f32 Window::scrollY() const {
		return scrollYHidden ? 0.0f : m_scrollY;
	}
	void Window::hideScrollX() {
		scrollXHidden = true;
	}
	void Window::hideScrollY() {
		scrollYHidden = true;
	}
	f32 Window::scrollXAndHide() {
		auto ret = scrollXHidden ? 0.0f : m_scrollX;
		hideScrollX();
		return ret;
	}
	f32 Window::scrollYAndHide() {
		auto ret = scrollYHidden ? 0.0f : m_scrollY;
		hideScrollY();
		return ret;
	}
	bool Window::update(f32 deltaTime) 	{
		if (bCursorCaptured) {
			std::cout << "warp mouse to 1,1 \n";
			if (SDL_WarpMouseGlobal(0, 0) < 0) {
				std::cout << "warp failed\n";
			}
			//SDL_WarpMouseInWindow(sdlWindowHandle, 1, 1);
		}

		scrollXHidden = false;
		scrollYHidden = false;

		// Keyboard
		std::swap(keyStates, prevKeyStates);
		const u8* keystate = SDL_GetKeyboardState(nullptr /* its 512 */);
		for (i32 i = 0; i < 512; ++i) {
			(*keyStates)[i] = keystate[i];
		}
		for (i32 i = 0; i < 512; ++i) {
			keyHidden[i] = false;
		}

		// Mouse
		prevCursorPos = cursorPos;
		std::swap(buttonStates, prevButtonStates);
		const u32 buttonMask = SDL_GetMouseState(&cursorPos[0], &cursorPos[1]);
		for (i32 i = 0; i < 5; ++i) {
			(*buttonStates)[i] = buttonMask & SDL_BUTTON(i + 1 /*button index starts with 11 for some reason! SDL2 moment*/);
		}
		for (i32 i = 0; i < 5; ++i) {
			buttonHidden[i] = false;
		}

		if (bChangedSize) {
			bChangedSize = false;
			SDL_SetWindowSize(sdlWindowHandle, size[0], size[1]);
		}

		// Events
		bool close{ false };
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				close = true;
				break;
			case SDL_KEYDOWN:
				break;
			}
		}
		SDL_GetWindowSize(sdlWindowHandle, (int*)&this->size[0], (int*)&this->size[1]);
		return close;
	}

	std::vector<KeyEvent> Window::getKeyEventsInOrder() const {
		std::vector<KeyEvent> ret;
		for (auto e : eventQ) {
			if (keyHidden[u32(e.scancode)]) {
				ret.push_back(e);
			}
		}
		return ret;
	}

	void* Window::getWindowHandleSDL() {
		return sdlWindowHandle;
	}

	VkSurfaceKHR Window::getSurface() {
		return surface;
	}
}