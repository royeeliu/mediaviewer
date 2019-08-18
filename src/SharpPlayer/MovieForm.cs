using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SharpPlayer
{
    public partial class MovieForm : Form
    {
        private MediaCore.CorePlayer player;
        private string mediaFileName;

        public MovieForm(string[] args)
        {
            InitializeComponent();

            if (args.Length == 1)
            {
                mediaFileName = args[0];
            }
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            player = new MediaCore.CorePlayer(this);

            if (!string.IsNullOrEmpty(mediaFileName))
            {
                LoadFile(mediaFileName);
            }
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            player.Dispose();
            player = null;
        }

        private void LoadFile(string fileName)
        {
            player.LoadFile(fileName);
        }
    }
}
