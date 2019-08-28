#include "framework.h"
#include "VideoViewer.h"
#include "ClientView.h"
#include "Common.h"
#include <thread>

#pragma comment(lib, "MediaApi.lib")

using namespace Leo;
using namespace Channels;

class VideoViewer::SourceThread
{
};

class VideoViewer::DecodeThread
{
};

class VideoViewer::RenderThread
{
public:
	RenderThread(Channels::ObjectQueue::OutputEnd&& output, HWND displayWindow)
		: m_output(std::move(output))
	{
		m_targetView.handle = (HWND)displayWindow;
		m_thread = std::thread([this] { WorkerThread(); });
	}

	~RenderThread()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

private:
	void WorkerThread()
	{
		CreateResource();
		DoThreadLoop();
		DestroyResource();
	}

	void DoThreadLoop()
	{
		CommandReply reply = CommandReply::None;

		while (reply != CommandReply::Exit)
		{
			ReferenceGuard<Referencable> object;
			int interruptCode = 0;
			auto waitResult = m_output.WaitPop(*object.GetAdress(), interruptCode);

			switch (waitResult)
			{
			case WaitResult::Ok:
				break;

			case WaitResult::Interrupted:
				reply = ProcessCommand(static_cast<Command>(interruptCode));
				break;

			default:
				{
					_ASSERTE(false);
				}
			}
		}
	}

	void CreateResource()
	{
		m_graphics = MAPI_Graphics_Create();

		MAPI_Error err{};
		MAPI_Graphics_Initialize(m_graphics, &m_targetView, &err);

		if (err.code != MAPI_NO_ERROR)
		{
			SHOW_ERROR_MESSAGE(L"Initialize video render failed: error = %d", err.code);
		}
	}

	void DestroyResource()
	{
		if (m_graphics != nullptr)
		{
			MAPI_Graphics_Destroy(&m_graphics);
		}
	}

	CommandReply ProcessCommand(Command cmd)
	{
		switch (cmd)
		{
		case Command::Shutdown:
			return CommandReply::Exit;

		case Command::Repaint:
			Render();
			break;
		}

		return CommandReply::None;
	}

	void Render()
	{
		MAPI_Error err{};
		MAPI_RenderTarget* target = MAPI_Graphics_BeginRender(m_graphics, &err);

		if (err.code != MAPI_NO_ERROR)
		{
			SHOW_ERROR_MESSAGE("MAPI_Graphics_BeginRender failed: {%d, %d, %s}", err.code, err.detail, err.message);
			return;
		}

		MAPI_RenderTarget_FillColor(target, &m_bkgColor, nullptr);
		MAPI_Graphic_EndRender(m_graphics, &target, &err);
	}

private:
	std::thread							m_thread;
	Channels::ObjectQueue::OutputEnd	m_output;
	MAPI_TargetView						m_targetView{};
	MAPI_Graphics*						m_graphics{ nullptr };
	MAPI_Color_ARGB						m_bkgColor{ 255, 0, 0, 0 };
};

VideoViewer::VideoViewer(ClientView& displayWindow)
	: m_displayWindow(displayWindow)
{
	m_connections.push_back(m_displayWindow.PaintEvent.connect([this] { Render(); }));
	m_connections.push_back(m_displayWindow.DestroyedEvent.connect([this] { Shutdown(); }));

	m_renderThread = std::make_unique<RenderThread>(ObjectQueue::OutputEnd(m_frameChannel), (HWND)m_displayWindow);

	Initialize();
}

VideoViewer::~VideoViewer()
{
	for (auto& item : m_connections)
	{
		item.disconnect();
	}
}

void VideoViewer::Initialize()
{
}

void VideoViewer::Shutdown()
{
	m_packtChannel.Reset(static_cast<int>(Command::Shutdown));
	m_packtChannel.Reset(static_cast<int>(Command::Shutdown));
}

void VideoViewer::Render()
{
	m_frameChannel.Interrupt(static_cast<int>(Command::Repaint));
}

