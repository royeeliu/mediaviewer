#pragma once

#include "Leo/Threading.h"
#include "Include/MediaApi.h"

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
		WaitCommand ,
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

		ID const cmd;

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

	enum class MediaObjectType
	{
		Invalid = 0,
		Packet,
		Frame,
		MediaDescriptor,
		Message,
	};

	struct MediaObject : Leo::Referencable
	{
		MediaObjectType const type;
		MediaObject(MediaObjectType type_) : type(type_){}
	};

	template<MediaObjectType type>
	struct MediaObjectImpl : MediaObject
	{
		MediaObjectImpl() : MediaObject(type){}
	};

	enum class MediaMessage : int
	{
		None,
		SessionStart,
		SessionEnd,
	};

	struct MediaMessageObject : MediaObjectImpl<MediaObjectType::Message>
	{
		MediaMessage const msg;
		MediaMessageObject(MediaMessage msg_) : msg{ msg_ }{}
	};

	struct MediaPacketObject : MediaObjectImpl<MediaObjectType::Packet>
	{
		std::shared_ptr<MAPI_MediaPacket> packet;
		MediaPacketObject(std::shared_ptr<MAPI_MediaPacket> packet_) : packet{ std::move(packet_) } {}
	};

	struct MediaFrameObject : MediaObjectImpl<MediaObjectType::Frame>
	{
		std::shared_ptr<MAPI_MediaFrame> frame;
		MediaFrameObject(std::shared_ptr<MAPI_MediaFrame> frame_) : frame{ std::move(frame_) } {}
	};

	struct MediaDescriptorObject : MediaObjectImpl<MediaObjectType::MediaDescriptor>
	{
		std::shared_ptr<MAPI_MediaDescriptor> media;
		MediaDescriptorObject(std::shared_ptr<MAPI_MediaDescriptor> media_) : media { std::move(media_) } {}
	};

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
	using ObjectQueueStatus = Leo::Threading::Channels::ObjectQueue::Status;
	using CommandChannel = Leo::Threading::Channels::AsyncChannel<CommandObject>;

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
