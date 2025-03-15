using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;

namespace DisplayAudio
{
    public partial class MainWindow : Window
    {
        [DllImport("AudioManager.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void GetSpeakerConfiguration(StringBuilder buffer, int bufferSize);

        public MainWindow()
        {
            InitializeComponent();
            LoadSpeakerConfig();
        }

        private void LoadSpeakerConfig()
        {
            StringBuilder buffer = new StringBuilder(512);
            GetSpeakerConfiguration(buffer, buffer.Capacity);

            SpeakerInfoText.Text = buffer.ToString();
        }
    }
}
