using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Input;
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
            this.StartStopCommand = new StartStopCommandImpl(this);
        }

        private void onWindowLoaded(object sender, RoutedEventArgs e)
        {
            videoHostControl.attach(VideoArea);
        }

        Win32.CHWndHostControl videoHostControl = new Win32.CHWndHostControl();
        public DirectX.CVideoPreview VideoPreview { get; protected set; }
        public StartStopCommandImpl StartStopCommand { get; protected set; }

        public class StartStopCommandImpl : ICommand
        {
            public StartStopCommandImpl(MainWindow mainWindow)
            {
                this.mainWindow = mainWindow;
            }

            MainWindow mainWindow;

            public bool CanExecute(object parameter)
            {
                return true;
            }

            // This event is never used
            public event EventHandler CanExecuteChanged { add { } remove { } }

            public void Execute(object parameter)
            {
                if (!mainWindow.VideoPreview.IsStarted)
                {
                    mainWindow.VideoPreview.start(mainWindow.videoHostControl.Handle, mainWindow.VideoArea.ActualWidth, mainWindow.VideoArea.ActualHeight);
                }
                else
                {
                    mainWindow.VideoPreview.stop();
                }
            }
        }
    }
}
