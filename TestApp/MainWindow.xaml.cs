using System;
using System.ComponentModel;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;

namespace TestApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        public MainWindow()
        {
            InitializeComponent();

            this.DataContext = this;
            this.StartStopCommand = new StartStopCommandImpl(this);
        }

        public DirectX.CVideoPreview VideoPreview {
            get { return videoPreview; }
            protected set {
                if (videoPreview != value)
                {
                    videoPreview = value;
                    raisePropertyChanged("VideoPreview");
                }
            }
        }
        DirectX.CVideoPreview videoPreview = null;

        public StartStopCommandImpl StartStopCommand { get; protected set; }

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
                mainWindow.PropertyChanged += (object o, PropertyChangedEventArgs e) => {
                    if (e.PropertyName == "VideoPreview")
                    {
                        if (CanExecuteChanged != null) { CanExecuteChanged(this, new EventArgs()); }
                    }
                };
            }

            MainWindow mainWindow;

            public bool CanExecute(object parameter)
            {
                return (mainWindow.VideoPreview != null);
            }

            public event EventHandler CanExecuteChanged;

            public void Execute(object parameter)
            {
                // Start/Stop preview by calling IsCameraActiveChanged callback
                mainWindow.IsCameraActive = !mainWindow.IsCameraActive;
            }
        }

        /// <summary>
        /// IsCameraActive property for ToggleButton control
        /// </summary>
        public bool IsCameraActive
        {
            get { return (videoPreview != null) ? videoPreview.IsStarted : false; }
            set
            {
                if (IsCameraActive == value) return;

                if (VideoPreview == null)
                {
                    VideoPreview = new DirectX.CVideoPreview();
                    VideoPreview.setup(VideoArea);
                }

                if (value)
                {
                    // Start preview when ToggleButton.IsChecked is True
                    VideoPreview.start();
                }
                else
                {
                    // Stop preview when ToggleButton.IsChecked is False
                    VideoPreview.stop();
                }
                raisePropertyChanged("IsCameraActive");
            }
        }

        void raisePropertyChanged(string name)
        {
            if (PropertyChanged != null) { PropertyChanged(this, new PropertyChangedEventArgs(name)); }
        }

        // Implementation of INotifyPropertyChanged
        public event PropertyChangedEventHandler PropertyChanged;
    }
}
