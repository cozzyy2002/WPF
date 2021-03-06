﻿using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Threading;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace TestApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        private static readonly log4net.ILog logger = log4net.LogManager.GetLogger(typeof(MainWindow));

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
                        if (logger.IsDebugEnabled) logger.DebugFormat("Current culture={0}", value.EnglishName);
                        break;
                }
            };
            this.Cultures = new List<CultureInfo>();
            foreach (string s in this.Settings.Cultures)
            {
                this.Cultures.Add(new CultureInfo(s));
            }

            this.DeviceCategories = new List<DirectX.CDevice.CCategory>(/*DirectX.CDevice.Categories*/)
            {
                                                            // Supported device categories are:
                DirectX.CDevice.VideoInputDeviceCategory,   // CVideoPreview
                DirectX.CDevice.AudioRendererCategory,      // CAudioPlayer
            };
            this.selectedCategory = DirectX.CDevice.VideoInputDeviceCategory;
            this.Devices = new ObservableCollection<DirectX.CDevice>();

            this.AudioFileName = @"C:\Windows\Media\flourish.mid";
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
                    if (logger.IsDebugEnabled) logger.DebugFormat("  " + dev.FriendlyName);
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
                cameraDevice = value;

                if (VideoPreview != null)
                {
                    VideoPreview.Dispose();
                    VideoPreview = null;
                }
                if (value != null)
                {
                    if (logger.IsDebugEnabled) logger.DebugFormat("Camera device: {0}", value.FriendlyName);
                    if (value.Is(DirectX.CDevice.VideoInputDeviceCategory))
                    {
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

        public static readonly DependencyProperty AudioFileNameProperty = DependencyProperty.Register("AudioFileName", typeof(string), typeof(MainWindow));
        public string AudioFileName {
            get { return (string)GetValue(AudioFileNameProperty); }
            set { SetValue(AudioFileNameProperty, value); }
        }

        private void onAudioFileSelectButtonClick(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.FileName = AudioFileName;
            dlg.Filter = "Audio files|*.wav;*.mid;*.mp3;*.wma|All files|*";
            if (dlg.ShowDialog(this) == true) { AudioFileName = dlg.FileName; }
        }

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
                return true;// (mainWindow.VideoPreview != null);
            }

            public event EventHandler CanExecuteChanged;

            public void Execute(object parameter)
            {
                // Start/Stop preview by calling IsCameraActiveChanged callback
                //mainWindow.IsCameraActive = !mainWindow.IsCameraActive;

                if (audioPlayer == null)
                {
                    if (mainWindow.cameraDevice.Is(DirectX.CDevice.AudioRendererCategory))
                    {
                        // Play audio file output to the device specified by CameraDevice
                        audioPlayer = new DirectX.CAudioPlayer(mainWindow.AudioFileName, mainWindow.CameraDevice);
                    }
                    else
                    {
                        // Play audio file output to the default audio renderer device
                        audioPlayer = new DirectX.CAudioPlayer(mainWindow.AudioFileName);
                    }

                    audioPlayer.PropertyChanged += (object s, PropertyChangedEventArgs e) =>
                    {
                        if (!audioPlayer.IsPlaying)
                        {
                            DispatcherTimer timer = new DispatcherTimer();
                            timer.Interval = new TimeSpan(0,0,2);
                            timer.Tick += new EventHandler((object o, EventArgs ev) =>
                                {
                                    timer.Stop();
                                    if (audioPlayer != null)
                                    {
                                        audioPlayer.rewind();
                                        audioPlayer.start();
                                    }
                                });
                            timer.Start();
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
