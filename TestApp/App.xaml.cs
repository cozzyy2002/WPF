using System;
using System.IO;
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
            using (Stream stream = File.OpenRead("log4net.config"))
            {
                log4net.Config.XmlConfigurator.Configure(stream);
            }

            TestApp.Properties.Settings settings = TestApp.Properties.Settings.Default;
            Thread.CurrentThread.CurrentCulture = Thread.CurrentThread.CurrentUICulture = settings.CultureInfo;

            DirectX.CRunningObjectHandler roh = new DirectX.CRunningObjectHandler();
            roh.PrintROT("TestApp.OnStartup");
        }
    }
}
