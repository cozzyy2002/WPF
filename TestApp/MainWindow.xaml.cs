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
            this.StartStopCommand = new StartStopCommandImpl(this);
            this.VideoPreview = new DirectX.CVideoPreview();
        }

        private void onWindowLoaded(object sender, RoutedEventArgs e)
        {
            VideoPreview.setup(VideoArea);
        }

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
                return (mainWindow.VideoPreview != null);
            }

            // This event is never used
            public event EventHandler CanExecuteChanged { add { } remove { } }

            public void Execute(object parameter)
            {
                if (!mainWindow.VideoPreview.IsStarted)
                {
                    mainWindow.VideoPreview.start();
                }
                else
                {
                    mainWindow.VideoPreview.stop();
                }
            }
        }
    }
}
