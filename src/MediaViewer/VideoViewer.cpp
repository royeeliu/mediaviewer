#include "framework.h"
#include "VideoViewer.h"
#include "ClientView.h"
#include "Common.h"
#include <thread>

#pragma comment(lib, "MediaApi.lib")

using namespace Leo;
using namespace Leo::Threading;

class VideoViewer::SourceThread
{
public:
	SourceThread(ObjectQueue::SendingEnd&& packetChannel, CommandChannel::ReceivingEnd&& commandChannel)
		: m_packetChannel(std::move(packetChannel))
		, m_commandChannel(std::move(commandChannel))
	{
		m_thread = std::thread([this] { WorkerThreadProc(); });
	}

	~SourceThread()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

private:
	void WorkerThreadProc()
	{
		CreateResource();
		DoWorkerThreadLoop();
		DestroyResource();
	}

	void DoWorkerThreadLoop()
	{
		ThreadLoopStatus status = ThreadLoopStatus::Continue;

		while (status != ThreadLoopStatus::Exit)
		{
			if (!m_started)
			{
				CommandObject cmd = m_commandChannel.Receive();
				status = ProcessCommand(cmd);
			}
			else
			{
				CommandObject cmd = m_commandChannel.Receive();
				status = ProcessCommand(cmd);
			}
		}
	}

	void CreateResource()
	{
	}

	void DestroyResource()
	{
	}

	ThreadLoopStatus ProcessCommand(CommandObject& cmd)
	{
		switch (cmd->cmd)
		{
		case Command::ID::LoadFile:
			LoadFile(dynamic_cast<LoadFileCommand*>(cmd.Get())->fileName);
			break;

		default: { _ASSERTE(false); }
		}

		return ThreadLoopStatus::Continue;
	}

	void LoadFile(std::wstring const& fileName)
	{
		WPRINTF(L"%s: %s\n", __FUNCTIONW__, fileName.c_str());
	}

private:
	std::thread							m_thread;
	ObjectQueue::SendingEnd				m_packetChannel;
	CommandChannel::ReceivingEnd		m_commandChannel;
	bool								m_started = false;
};

class VideoViewer::DecodeThread
{
};

class VideoViewer::RenderThread
{
public:
	RenderThread(ObjectQueue::ReceivingEnd&& frameChannel, HWND displayWindow)
		: m_frameChannel(std::move(frameChannel))
	{
		m_targetView.handle = (HWND)displayWindow;
		m_thread = std::thread([this] { WorkerThreadProc(); });
	}

	~RenderThread()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

private:
	void WorkerThreadProc()
	{
		CreateResource();
		DoThreadLoop();
		DestroyResource();
	}

	void DoThreadLoop()
	{
		ThreadLoopStatus status = ThreadLoopStatus::Continue;

		while (status != ThreadLoopStatus::Exit)
		{
			ReferenceGuard<Referencable> object;
			int interruptCode = 0;
			auto waitResult = m_frameChannel.Receive(*object.GetAdress(), interruptCode);

			switch (waitResult)
			{
			case WaitResult::Ok:
				break;

			case WaitResult::Interrupted:
				status = ProcessInterrupt(static_cast<InterruptCode>(interruptCode));
				break;

			default: { _ASSERTE(false); }
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

	ThreadLoopStatus ProcessInterrupt(InterruptCode code)
	{
		switch (code)
		{
		case InterruptCode::None:
			break;

		case InterruptCode::SendCommand:
			break;

		case InterruptCode::Shutdown:
			return ThreadLoopStatus::Exit;

		case InterruptCode::Repaint:
			Render();
			break;

		default: { _ASSERTE(false); }
		}

		return ThreadLoopStatus::Continue;
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
	ObjectQueue::ReceivingEnd			m_frameChannel;
	MAPI_TargetView						m_targetView{};
	MAPI_Graphics*						m_graphics{ nullptr };
	MAPI_Color_ARGB						m_bkgColor{ 255, 0, 0, 0 };
};

VideoViewer::VideoViewer(ClientView& displayWindow)
	: m_displayWindow(displayWindow)
{
	m_connections.push_back(m_displayWindow.PaintEvent.connect([this] { Render(); }));
	m_connections.push_back(m_displayWindow.DestroyedEvent.connect([this] { Shutdown(); }));

	CommandChannel sourceCommandChannel{};
	m_sourceCommandSender = CommandChannel::SendingEnd(sourceCommandChannel);

	m_sourceThread = std::make_unique<SourceThread>(
		ObjectQueue::SendingEnd(m_packtChannel),
		CommandChannel::ReceivingEnd(sourceCommandChannel));
	m_renderThread = std::make_unique<RenderThread>(
		ObjectQueue::ReceivingEnd(m_frameChannel),
		(HWND)m_displayWindow);

	Initialize();
}

VideoViewer::~VideoViewer()
{
	for (auto& item : m_connections)
	{
		item.disconnect();
	}
}

void VideoViewer::LoadFile(wchar_t const* fileName)
{
	_ASSERTE(fileName != nullptr);
	auto cmd = MakeReferenceGuard(new LoadFileCommand{});
	cmd->fileName = fileName;
	m_sourceCommandSender.SendAsync(CommandObject::Attach(cmd.Detach()));
}

void VideoViewer::Initialize()
{
}

void VideoViewer::Shutdown()
{
}

void VideoViewer::Render()
{
	m_frameChannel.InterruptReceivingEnd(static_cast<int>(InterruptCode::Repaint));
}

