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

            this.DataContext = this;
            this.VideoPreview = new DirectX.CVideoPreview();
        }

        private void onWindowLoaded(object sender, RoutedEventArgs e)
        {
            videoHostControl.attach(VideoArea);
        }

        Win32.CHWndHostControl videoHostControl = new Win32.CHWndHostControl();
        public DirectX.CVideoPreview VideoPreview { get; protected set; }

        private void onStartButtonClick(object sender, RoutedEventArgs e)
        {
            if (!VideoPreview.IsStarted)
            {
                VideoPreview.start(videoHostControl.Handle, VideoArea.ActualWidth, VideoArea.ActualHeight);
            }
            else
            {
                VideoPreview.stop();
            }
        }
    }
}
