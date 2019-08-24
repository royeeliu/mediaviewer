#pragma once

#include <Unknwn.h>

namespace mapi {

template<class Interface>
void SafeRelease(Interface*& ptr)
{
	IUnknown* pUnk = static_cast<IUnknown*>(ptr);
	ptr = nullptr;

	if (pUnk != nullptr)
	{
		pUnk->Release();
	}
}

} // End of namespace
