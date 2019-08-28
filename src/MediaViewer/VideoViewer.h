#pragma once

#include "Include/MediaApi.h"
#include "Leo/Threading.h"

class ClientView;

class VideoViewer
{
	class SourceThread;
	class DecodeThread;
	class RenderThread;

	enum class InterruptCode : int
	{
		// control command
		None = 0,
		SendCommand,
		Shutdown,
		Repaint,
	};

	enum class ThreadLoopStatus : int
	{
		Continue	= 0,
		Exit		,
	};

	enum class Status : int
	{
		Idl = 0,
		Playing,
		Paused,
	};

	struct Command : Leo::Referencable
	{
		enum class ID
		{
			None = 0,
			LoadFile,
		};

		ID cmd = ID::None;

		Command(ID cmd_) : cmd(cmd_){}
	};

	template<Command::ID id>
	struct CommandImpl : Command
	{
		CommandImpl() : Command(id) {};
	};

	struct LoadFileCommand : CommandImpl<Command::ID::LoadFile>
	{
		std::wstring fileName;
	};

	using CommandObject = Leo::ReferenceGuard<Command>;

public:
	explicit
	VideoViewer(ClientView& displayWindow);
	~VideoViewer();

	void LoadFile(wchar_t const* fileName);

private:
	void Initialize();
	void Shutdown();
	void Render();

private:
	using SignalConnection = boost::signals2::connection;
	using SignalConnectionArray = std::vector<SignalConnection>;
	using ObjectQueue = Leo::Threading::Channels::ObjectQueue;
	using CommandChannel = Leo::Threading::Channels::AsyncChannel<CommandObject>;
	using WaitResult = Leo::Threading::Channels::WaitResult;

	ClientView&						m_displayWindow;
	SignalConnectionArray			m_connections;
	std::unique_ptr<SourceThread>	m_sourceThread;
	std::unique_ptr<DecodeThread>	m_decodeThread;
	std::unique_ptr<RenderThread>	m_renderThread;
	ObjectQueue						m_packtChannel;
	ObjectQueue						m_frameChannel;
	CommandChannel::SendingEnd		m_sourceCommandSender;
	Status							m_status = Status::Idl;
};
