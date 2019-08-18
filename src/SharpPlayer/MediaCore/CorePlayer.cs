using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace SharpPlayer.MediaCore
{
    public class CorePlayer : IDisposable
    {
        private IntPtr impl;

        public CorePlayer(Form form)
        {
            impl = MediaCoreApi.media_player_create();
            MediaCoreApi.media_player_initialize(form.Handle);
        }

        public void LoadFile(string fileName)
        {
            MediaCoreApi.media_player_load_file(fileName);
        }

        public void Dispose()
        {
            if (impl != IntPtr.Zero)
            {
                MediaCoreApi.media_player_destory(impl);
                impl = IntPtr.Zero;
            }
        }
    }
}
