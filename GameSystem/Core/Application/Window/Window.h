#pragma once

class Window {
public:
	struct WindowProps
	{
		WindowProps(const wchar_t* t = L"Game Suite",
			unsigned int w = 1280,
			unsigned int h = 720)
			: title(t), width(w), height(h) {}
		const wchar_t* title;
		unsigned int width;
		unsigned int height;
	};
	virtual ~Window() = default;
	virtual bool Update() = 0;
	static std::unique_ptr<Window> Create(WindowProps props = WindowProps());
};