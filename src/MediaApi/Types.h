#pragma once

#include "framework.h"

namespace mapi {

using Error = MAPI_Error;
using ERROR_CODE = MAPI_ERROR_CODE;

} // End of namespace


#define RETURN_VOID_IF(exp)		do { if(exp) return; } while(0)
