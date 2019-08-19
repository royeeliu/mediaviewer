#pragma once

#include "framework.h"
#include "Include/MediaApi/Types.h"

namespace mapi {

using Error = MAPI_Error;
using ERROR_CODE = MAPI_ERROR_CODE;
using View = MAPI_View;

class ImageView;

} // End of namespace


#define RETURN_VOID_IF(exp)		do { if(exp) return; } while(0)
