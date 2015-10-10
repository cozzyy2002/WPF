using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Threading;
using System.Windows;
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

            this.Settings = Properties.Settings.Default;
            this.Settings.PropertyChanged += (object sender, PropertyChangedEventArgs e) =>
            {
                switch (e.PropertyName) {
                    case "CultureInfo":
                        CultureInfo value = Settings.CultureInfo;
                        Console.WriteLine("Current culture={0}", value.EnglishName);
                        Dispatcher.BeginInvoke((Action)(() => {
                            Thread.CurrentThread.CurrentCulture = value;
                            Thread.CurrentThread.CurrentUICulture = value;
                            MessageBox.Show(DateTime.Now.ToString("F"), value.NativeName, MessageBoxButton.YesNoCancel, MessageBoxImage.Information);
                        }));
                        break;
                }
            };
            this.Cultures = new List<CultureInfo>();
            foreach (string s in this.Settings.Cultures)
            {
                this.Cultures.Add(new CultureInfo(s));
            }

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

        public Properties.Settings Settings { get; protected set; }
        public List<CultureInfo> Cultures { get; protected set; }

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

        private void onWindowClosed(object sender, EventArgs e)
        {
            Settings.Save();
        }
    }
}
