using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows;
using System.Windows.Data;
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

        ObjectDataProvider provider;

        public string LanguageName
        {
            get { return languageName; }
            set
            {
                if (languageName != value)
                {
                    languageName = value;
                    try
                    {
                        CultureInfo ci = new CultureInfo(value);
                        Console.WriteLine("Changing Language to {0}", ci);
                        TestApp.Properties.Resources.Culture = ci;
                    }
                    catch(Exception ex)
                    {
                        Console.WriteLine("{0}: Language={1}", ex.Message, value);
                    }
                }
            }
        }
        string languageName = "en-US";

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

        /// <summary>
        /// IsCameraActive property for ToggleButton control
        /// </summary>
        public bool IsCameraActive
        {
            get { return isCameraActive; }
            set
            {
                isCameraActive = value;
                if (isCameraActive)
                {
                    // Start preview when ToggleButton.IsChecked is True
                    VideoPreview.start();
                }
                else
                {
                    // Stop preview when ToggleButton.IsChecked is False
                    VideoPreview.stop();
                }
            }
        }

        bool isCameraActive = false;
    }
}
