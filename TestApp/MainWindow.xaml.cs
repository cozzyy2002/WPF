using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;

namespace TestApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void onWindowLoaded(object sender, RoutedEventArgs e)
        {
            videoHostControl = new ControlHost(VideoArea.ActualWidth, VideoArea.ActualHeight);
            VideoArea.Child = videoHostControl;
        }

        ControlHost videoHostControl;
        DirectX.CVideoPreview videoPreview = new DirectX.CVideoPreview();

        private void onStartButtonClick(object sender, RoutedEventArgs e)
        {
            videoPreview.start(videoHostControl.Handle, VideoArea.ActualWidth, VideoArea.ActualHeight);
        }
    }

    public class ControlHost : HwndHost
    {
        IntPtr hwndHost;
        int hostHeight, hostWidth;

        public ControlHost(double height, double width)
        {
            hostHeight = (int)height;
            hostWidth = (int)width;
        }

        internal const int
          WS_CHILD = 0x40000000,
          WS_VISIBLE = 0x10000000,
          LBS_NOTIFY = 0x00000001,
          HOST_ID = 0x00000002,
          LISTBOX_ID = 0x00000001,
          WS_VSCROLL = 0x00200000,
          WS_BORDER = 0x00800000;

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            hwndHost = IntPtr.Zero;

            hwndHost = CreateWindowEx(0, "static", "",
                                      WS_CHILD | WS_VISIBLE,
                                      0, 0,
                                      hostWidth, hostHeight,
                                      hwndParent.Handle,
                                      (IntPtr)HOST_ID,
                                      IntPtr.Zero,
                                      0);

            return new HandleRef(this, hwndHost);
        }

        //PInvoke declarations
        [DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Unicode)]
        internal static extern IntPtr CreateWindowEx(int dwExStyle,
                                                      string lpszClassName,
                                                      string lpszWindowName,
                                                      int style,
                                                      int x, int y,
                                                      int width, int height,
                                                      IntPtr hwndParent,
                                                      IntPtr hMenu,
                                                      IntPtr hInst,
                                                      [MarshalAs(UnmanagedType.AsAny)] object pvParam);

        protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            handled = false;
            return IntPtr.Zero;
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            DestroyWindow(hwnd.Handle);
        }

        [DllImport("user32.dll", EntryPoint = "DestroyWindow", CharSet = CharSet.Unicode)]
        internal static extern bool DestroyWindow(IntPtr hwnd);

    }
}
