#pragma once

class MainFrame;
class VideoView;

class Application
{
public:
	static Application Current;

private:
	Application();
	~Application();

	Application(Application const&) = delete;
	Application& operator=(Application const&) = delete;

public:
	void Initialize(HINSTANCE hinst);
	void Run(int show);
	void ShowErrorMessage(const wchar_t* format, ...);
	
	HINSTANCE GetInstance() const { return m_hinstance; }

private:
	HINSTANCE					m_hinstance = nullptr;
	HWND						m_hwndConsole = nullptr;
	std::unique_ptr<MainFrame>	m_mainFrame;
	std::unique_ptr<VideoView>	m_videoView;
};

