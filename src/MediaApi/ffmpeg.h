#pragma once

extern "C" {

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

}

#pragma comment(lib, "avformat-x64.lib")
#pragma comment(lib, "avcodec-x64.lib")
#pragma comment(lib, "avutil-x64.lib")
#pragma comment(lib, "swscale-x64.lib")
