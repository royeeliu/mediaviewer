#include "framework.h"
#include "VideoViewer.h"
#include "ClientView.h"
#include "RenderThread.h"
#include "DecodeThread.h"
#include "SourceThread.h"
#include "Common.h"

using namespace Leo;
using namespace Leo::Threading;

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

