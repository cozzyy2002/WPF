using System.Windows;

namespace ie.WPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public string TargetUrl
        {
            get { return (string)GetValue(TargetUrlProperty); }
            set { SetValue(TargetUrlProperty, value); }
        }

        public static readonly DependencyProperty TargetUrlProperty
            = DependencyProperty.Register("TargetUrl", typeof(string), typeof(MainWindow));

        public string CurrentUrl
        {
            get { return (string)GetValue(CurrentUrlProperty); }
            private set { SetValue(CurrentUrlProperty, value); }
        }

        public static readonly DependencyProperty CurrentUrlProperty
            = DependencyProperty.Register("CurrentUrl", typeof(string), typeof(MainWindow));

        public MainWindow()
        {
            InitializeComponent();

            this.TargetUrl = "https://beta.ucs.ricoh.com/util/login";
            this.webBrowser.LoadCompleted += WebBrowser_LoadCompleted;
        }

        private void WebBrowser_LoadCompleted(object sender, System.Windows.Navigation.NavigationEventArgs e)
        {
            CurrentUrl = e.Uri.AbsoluteUri + ": LoadCompleted";
        }

        private void GoButton_Click(object sender, RoutedEventArgs e)
        {
            CurrentUrl = this.TargetUrl + ": Navigating";
            webBrowser.Navigate(this.TargetUrl);
        }
    }
}
