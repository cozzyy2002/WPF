using System;
using System.Threading;
using System.Windows;

namespace TestApp
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private void onStartup(object sender, StartupEventArgs e)
        {
            TestApp.Properties.Settings settings = TestApp.Properties.Settings.Default;
            Thread.CurrentThread.CurrentCulture = Thread.CurrentThread.CurrentUICulture = settings.CultureInfo;
        }
    }
}
