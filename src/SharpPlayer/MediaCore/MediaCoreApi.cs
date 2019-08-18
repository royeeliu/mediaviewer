using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace SharpPlayer.MediaCore
{
    class MediaCoreApi
    {
        [DllImport("MediaCore.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr media_player_create();

        [DllImport("MediaCore.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void media_player_destory(IntPtr ptr);

        [DllImport("MediaCore.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 media_player_initialize(IntPtr view);

        [DllImport("MediaCore.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 media_player_load_file([MarshalAs(UnmanagedType.LPUTF8Str)]string url);
    }
}
