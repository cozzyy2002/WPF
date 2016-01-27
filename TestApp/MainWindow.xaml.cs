using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
                        break;
                }
            };
            this.Cultures = new List<CultureInfo>();
            foreach (string s in this.Settings.Cultures)
            {
                this.Cultures.Add(new CultureInfo(s));
            }

            this.DeviceCategories = new List<DirectX.CDevice.CCategory>(DirectX.CDevice.Categories);
            this.selectedCategory = DirectX.CDevice.VideoInputDeviceCategory;
            this.Devices = new ObservableCollection<DirectX.CDevice>();
        }

        public List<DirectX.CDevice.CCategory> DeviceCategories { get; protected set; }
        public DirectX.CDevice.CCategory SelectedCategory
        {
            get { return selectedCategory; }
            set
            {
                Devices.Clear();
                selectedCategory = value;
                foreach (DirectX.CDevice dev in DirectX.CDevice.getDevices(value))
                {
                    Console.WriteLine("  " + dev.FriendlyName);
                    Devices.Add(dev);
                }
                CameraDevice = (0 < Devices.Count) ? Devices[0] : null;
            }
        }
        DirectX.CDevice.CCategory selectedCategory;

        public ObservableCollection<DirectX.CDevice> Devices { get; set; }

        public DirectX.CDevice CameraDevice {
            get { return cameraDevice; }
            set
            {
                if (VideoPreview != null)
                {
                    VideoPreview.Dispose();
                    VideoPreview = null;
                }
                if (value != null)
                {
                    Console.WriteLine("Camera device: {0}", value.FriendlyName);
                    if (value.Is(DirectX.CDevice.VideoInputDeviceCategory))
                    {
                        cameraDevice = value;
                        VideoPreview = new DirectX.CVideoPreview();
                        VideoPreview.setup(CameraDevice, VideoArea);
                    }
                }
                raisePropertyChanged("CameraDevice");
            }
        }
        DirectX.CDevice cameraDevice;

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
                //mainWindow.IsCameraActive = !mainWindow.IsCameraActive;

                if (audioPlayer == null)
                {
                    audioPlayer = new DirectX.CAudioPlayer(
                        @"C:\Windows\Media\ringout.wav"
                        //@"C:\cozzy\Music\Al DiMeola\Elegant Gypsy\01 Flight over Rio.wma"
                        );
                    audioPlayer.PropertyChanged += (object s, PropertyChangedEventArgs e) =>
                    {
                        if (!audioPlayer.IsPlaying)
                        {
                            Thread.Sleep(2000);
                            audioPlayer.rewind();
                        }
                    };
                    audioPlayer.start();
                }
                else
                {
                    audioPlayer.stop();
                    audioPlayer.Dispose();
                    audioPlayer = null;
                }
            }

            DirectX.CAudioPlayer audioPlayer = null;
        }

        /// <summary>
        /// IsCameraActive property for ToggleButton control
        /// </summary>
        public bool IsCameraActive
        {
            get { return (videoPreview != null) ? videoPreview.IsStarted : false; }
            set
            {
                if ((IsCameraActive == value) || (VideoPreview ==null)) return;

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

        private void onToolButtonClick(object sender, RoutedEventArgs e)
        {
            ToolWindow win = new ToolWindow();
            win.Owner = this;
            win.Show();
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

        private void onVideoAreaSizeChanged(object sender, SizeChangedEventArgs e)
        {
            if(VideoPreview != null) VideoPreview.setSize(e.NewSize);
        }
    }
}
