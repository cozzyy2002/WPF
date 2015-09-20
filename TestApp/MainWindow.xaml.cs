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
            videoHostControl.attach(VideoArea);
        }

        Win32.CHWndHostControl videoHostControl = new Win32.CHWndHostControl();
        DirectX.CVideoPreview videoPreview = new DirectX.CVideoPreview();

        private void onStartButtonClick(object sender, RoutedEventArgs e)
        {
            videoPreview.start(videoHostControl.Handle, VideoArea.ActualWidth, VideoArea.ActualHeight);
        }
    }
}
