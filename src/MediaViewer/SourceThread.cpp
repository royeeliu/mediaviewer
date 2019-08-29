#include "framework.h"
#include "SourceThread.h"
#include "Common.h"
#include "Dispatcher.h"
#include "Include/Helper/StringHelper.h"
#include "Include/MediaApiHelper.h"

using namespace StringHelper;
using namespace MediaApiHelper;

VideoViewer::SourceThread::SourceThread(ObjectQueue::SendingEnd&& packetChannel, CommandChannel::ReceivingEnd&& commandChannel)
	: m_packetChannel(std::move(packetChannel))
	, m_commandChannel(std::move(commandChannel))
{
	m_thread = std::thread([this] { WorkerThreadProc(); });
}

VideoViewer::SourceThread::~SourceThread()
{
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void VideoViewer::SourceThread::WorkerThreadProc()
{
	CreateResource();
	DoWorkerThreadLoop();
	DestroyResource();
}

void VideoViewer::SourceThread::DoWorkerThreadLoop()
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

void VideoViewer::SourceThread::CreateResource()
{
	_ASSERTE(m_mediaSource == nullptr);
	m_mediaSource = MAPI_MediaSource_Create();
}

void VideoViewer::SourceThread::DestroyResource()
{
	MAPI_MediaSource_Destroy(&m_mediaSource);
	_ASSERTE(m_mediaSource == nullptr);
}

VideoViewer::ThreadLoopStatus VideoViewer::SourceThread::ProcessCommand(CommandObject& cmd)
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

void VideoViewer::SourceThread::LoadFile(std::wstring const& fileName)
{
	_ASSERTE(m_mediaSource != nullptr);
	MAPI_Error err{};
	MAPI_MediaSource_LoadFile(m_mediaSource, W2U8(fileName).c_str(), &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"MAPI_MediaSource_LoadFile failed: %s, %s", 
			FormatError(err), 
			fileName.c_str());
		return;
	}

	WPRINTF(L"File opened: %s\n", fileName.c_str());
	uint32_t const streamCount = MAPI_MediaSource_GetStreamCount(m_mediaSource);

	for (uint32_t i = 0; i < streamCount; i++)
	{
		MAPI_SourceStream* stream = MAPI_MediaSource_GetStream(m_mediaSource, i);
		uint32_t id = MAPI_SourceStream_GetId(stream);
		MAPI_MediaType type = MAPI_SourceStream_GetMediaType(stream);
		MAPI_SourceStream_Destroy(&stream);

		WPRINTF(L"##[%d] Stream %d: %s\n", i, id, U8ToW(MediaApiHelper::MediaTypeName(type)).c_str());
	}

	m_started = true;
}
