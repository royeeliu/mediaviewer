#pragma once

#include "Types.h"
#include "Diagnisis.h"

namespace mapi {

inline bool operator==(SIZE const& l, SIZE const& r)
{
	return l.cx == r.cx && l.cy == r.cy;
}

inline bool operator!=(SIZE const& l, SIZE const& r)
{
	return l.cx != r.cx || l.cy != r.cy;
}

} // End of namespace mapi

#define RETURN_VOID_IF(exp)		do { if(exp) return; } while(0)
#define BREAK_IF(exp)			if (exp) break
