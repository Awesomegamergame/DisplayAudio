using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Input;

namespace DisplayAudio
{
    public partial class MainWindow : Window
    {
        private System.Windows.Forms.NotifyIcon _notifyIcon;
        private const int HOTKEY_ID = 9000; // Unique hotkey ID
        private const int MOD_NOREPEAT = 0x4000;
        private const int MOD_ALT = 0x0001; // Alt key modifier
        private const int VK_A = 0x41; // 'A' key

        [DllImport("user32.dll")]
        private static extern bool RegisterHotKey(IntPtr hWnd, int id, int fsModifiers, int vk);

        [DllImport("user32.dll")]
        private static extern bool UnregisterHotKey(IntPtr hWnd, int id);

        [DllImport("AudioManager.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void GetSpeakerConfiguration(StringBuilder buffer, int bufferSize);

        public MainWindow()
        {
            InitializeComponent();
            LoadSpeakerConfig();
            this.StateChanged += MainWindow_StateChanged;
            Loaded += OnWindowLoaded; // Register hotkey only after the window has loaded
            InitializeTrayIcon();
        }

        private void LoadSpeakerConfig()
        {
            StringBuilder buffer = new StringBuilder(512);
            GetSpeakerConfiguration(buffer, buffer.Capacity);

            SpeakerInfoText.Text = buffer.ToString();
        }

        private void OnWindowLoaded(object sender, RoutedEventArgs e)
        {
            RegisterGlobalHotkey();
            string[] args = Environment.GetCommandLineArgs();
            if (args.Length > 1 && args[1].Equals("--minimized", StringComparison.OrdinalIgnoreCase))
            {
                HideToTray();
            }
            else
            {
                this.WindowState = WindowState.Normal;
            }
        }

        private void InitializeTrayIcon()
        {
            _notifyIcon = new System.Windows.Forms.NotifyIcon
            {
                Icon = Properties.Resources.Icon,
                Visible = false,
                Text = "DisplayAudio"
            };

            _notifyIcon.Click += (sender, args) =>
            {
                ShowWindow();
            };
        }

        private void RegisterGlobalHotkey()
        {
            IntPtr handle = new System.Windows.Interop.WindowInteropHelper(this).Handle;
            RegisterHotKey(handle, HOTKEY_ID, MOD_ALT | MOD_NOREPEAT, VK_A);

            var source = System.Windows.Interop.HwndSource.FromHwnd(handle);
            source.AddHook(WndProc);
        }

        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if (msg == 0x0312 && wParam.ToInt32() == HOTKEY_ID)
            {
                ShowWindow();
                handled = true;
            }
            return IntPtr.Zero;
        }

        private void ShowWindow()
        {
            this.Show();
            this.WindowState = WindowState.Normal;
            this.Activate();
            LoadSpeakerConfig();
            _notifyIcon.Visible = false;
        }

        private void HideToTray()
        {
            this.Hide();
            _notifyIcon.Visible = true;
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.Key == Key.Escape)
            {
                HideToTray();
                e.Handled = true;
            }
            base.OnKeyDown(e);
        }

        protected override void OnClosed(EventArgs e)
        {
            UnregisterHotKey(new System.Windows.Interop.WindowInteropHelper(this).Handle, HOTKEY_ID);
            _notifyIcon.Dispose();
            base.OnClosed(e);
        }

        private void MainWindow_StateChanged(object sender, EventArgs e)
        {
            if (this.WindowState == WindowState.Minimized)
            {
                HideToTray();
            }
        }
    }
}
