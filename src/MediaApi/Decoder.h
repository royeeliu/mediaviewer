#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "DecoderInterface.h"

namespace mapi {

class Decoder : Uncopyable 
{
public:
	Decoder();
	~Decoder();

public:
	void Initialize(MediaDescriptor const& media, Graphics const* graphics, Error& err);
	void SendPacket(MediaPacket const& packet, Error& err);
	std::shared_ptr<MediaFrame> ReceiveFrame(Error& err);
	std::shared_ptr<MediaDescriptor> GetMediaDescriptor() const;

private:
	std::unique_ptr<DecoderInterface> m_impl;
};

} // End of namespace mapi
