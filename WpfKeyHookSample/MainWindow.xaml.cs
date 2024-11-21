using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;

namespace WpfKeyHookSample
{
    public partial class MainWindow : Window
    {
        const uint WM_USER        = 0x0400;
        const uint WM_USERMESSAGE = WM_USER + 50;
        const int  virtualKey     = 'A';

        [DllImport("KeyHook.dll")]
        public static extern void Set(IntPtr hWnd, int virtualKey);
        [DllImport("KeyHook.dll")]
        public static extern void Reset();

        public MainWindow()
        {
            InitializeComponent();

            var handle = new WindowInteropHelper(this).EnsureHandle();
            HwndSource.FromHwnd(handle).AddHook(new HwndSourceHook(WindowProcedure));
        }

        public IntPtr Handle => new WindowInteropHelper(this).Handle;

        private IntPtr WindowProcedure(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if (msg == WM_USERMESSAGE) {
                Debug.WriteLine($"WM_USERMESSAGE({wParam},{lParam})");

                const uint isShiftKeyPressed   = 0x01 << 0;
                const uint isControlKeyPressed = 0x01 << 1;
                const uint isAltKeyPressed     = 0x01 << 2;

                if ((lParam & isShiftKeyPressed  ) != 0 &&
                    (lParam & isControlKeyPressed) != 0 &&
                    (lParam & isAltKeyPressed    ) != 0)
                    ShowNormal();
            }
            return IntPtr.Zero;
        }

        private void ShowNormal()
        {
            if (WindowState == WindowState.Minimized)
                WindowState = WindowState.Normal;
            if (!IsVisible)
                Show();
            Activate();
        }

        private void OnLoaded(object sender, RoutedEventArgs e) => Set(Handle, virtualKey);
        private void OnClosed(object sender, EventArgs       e) => Reset();
    }
}