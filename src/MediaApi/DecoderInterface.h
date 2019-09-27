#pragma once

#include "Types.h"

namespace mapi {

class DecoderInterface
{
public:
	virtual void Initialize(MediaDescriptor const& media, Graphics const* graphics, Error& err) = 0;
	virtual void SendPacket(MediaPacket const& packet, Error& err) = 0;
	virtual std::shared_ptr<MediaFrame> ReceiveFrame(Error& err) = 0;
	virtual std::shared_ptr<MediaDescriptor> GetOutputMediaDescriptor() const = 0;

public:
	virtual ~DecoderInterface() {}
};

} // End of namespace mapi

