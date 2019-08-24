#pragma once

#include "Types.h"
#include "Diagnisis.h"

namespace mapi {


} // End of namespace mapi

#define RETURN_VOID_IF(exp)		do { if(exp) return; } while(0)
#define BREAK_IF(exp)			if (exp) break
