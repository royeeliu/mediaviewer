#pragma once

#include "Include/MediaApi.h"
#include "Leo/Channels.h"

class ClientView;

class VideoViewer
{
	class SourceThread;
	class DecodeThread;
	class RenderThread;

	enum class Command : int
	{
		// control command
		None = 0,
		Shutdown = 1,

		// render command
		Repaint = 100,
	};

	enum class CommandReply : int
	{
		None	= 0,
		Exit	= 1,
	};

public:
	explicit
	VideoViewer(ClientView& displayWindow);
	~VideoViewer();

private:
	void Initialize();
	void Shutdown();
	void Render();

private:

private:
	using SignalConnection = boost::signals2::connection;
	using SignalConnectionArray = std::vector<SignalConnection>;

	ClientView&						m_displayWindow;
	SignalConnectionArray			m_connections;
	std::unique_ptr<SourceThread>	m_sourceThread;
	std::unique_ptr<DecodeThread>	m_decodeThread;
	std::unique_ptr<RenderThread>	m_renderThread;
	Leo::Channels::ObjectQueue		m_packtChannel;
	Leo::Channels::ObjectQueue		m_frameChannel;
};
