using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace TestApp
{
    /// <summary>
    /// Interaction logic for ToolWindow.xaml
    /// </summary>
    public partial class ToolWindow : Window
    {
        public ToolWindow()
        {
            InitializeComponent();

            this.DataContext = this;
            this.Settings = Properties.Settings.Default;

            this.CultureInfoList = new List<CultureInfo>(
                    Settings.Cultures.Cast<string>().Select<string, CultureInfo>(name => new CultureInfo(name))
                );
        }

        public Properties.Settings Settings { get; protected set; }

        public List<CultureInfo> CultureInfoList { get; protected set; }

        public class CultureInfo : INotifyPropertyChanged
        {
            public CultureInfo() { }
            public CultureInfo(string name) { this.Name = name; }

            public string Name
            {
                get { return (inner == null) ? "" : inner.Name; }
                set
                {
                    Console.WriteLine("CultureInfo: Changing name: '{0}' -> '{1}'", (inner == null) ? "null" : Name, value);
                    if ((inner == null) || (inner.Name != value)) {
                        inner = new System.Globalization.CultureInfo(value);
                        if (PropertyChanged != null)
                        {
                            foreach (string name in propertyNames)
                            {
                                PropertyChanged(this, new PropertyChangedEventArgs(name));
                            }
                        }
                    }
                }
            }

            static readonly string[] propertyNames = { "Name", "DisplayName", "EnglishName", "NativeName", "LCID", };

            public string DisplayName { get { return (inner == null) ? "" : inner.DisplayName; } }
            public string EnglishName { get { return (inner == null) ? "" : inner.EnglishName; } }
            public string NativeName { get { return (inner == null) ? "" : inner.NativeName; } }
            public int LCID { get { return (inner == null) ? 0 : inner.LCID; } }

            System.Globalization.CultureInfo inner = null;

            public event PropertyChangedEventHandler PropertyChanged;
        }
    }
}
