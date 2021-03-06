﻿using System;
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
        private static readonly log4net.ILog logger = log4net.LogManager.GetLogger(typeof(ToolWindow));

        public ToolWindow()
        {
            InitializeComponent();

            this.DataContext = this;
            this.Settings = Properties.Settings.Default;

            this.DialogBoxTypes = new List<DialogBoxType>(
                Enum.GetValues(typeof(DialogBoxType)).Cast<DialogBoxType>());
            this.CultureInfoList = new List<CultureInfo>(
                    Settings.Cultures.Cast<string>().Select<string, CultureInfo>(name => new CultureInfo(name))
                );
        }

        public Properties.Settings Settings { get; protected set; }

        public List<DialogBoxType> DialogBoxTypes { get; protected set; }
        public DialogBoxType DialogBoxType { get; set; }
        public List<CultureInfo> CultureInfoList { get; protected set; }

        public class CultureInfo : INotifyPropertyChanged, IFormattable
        {
            public CultureInfo() { }
            public CultureInfo(string name) { this.Name = name; }
            public CultureInfo(int lcid) { this.LCID = lcid; }

            public string Name
            {
                get { return (inner == null) ? "" : inner.Name; }
                set
                {
                    if ((inner == null) || (inner.Name != value)) {
                        if (logger.IsDebugEnabled) logger.DebugFormat("CultureInfo: Changing Name: '{0}' -> '{1}'", Name, value);
                        inner = new System.Globalization.CultureInfo(value);
                        onPropertyChanged();
                    }
                }
            }

            public int LCID
            {
                get { return (inner == null) ? 0 : inner.LCID; }
                set
                {
                    if ((inner == null) || (inner.LCID != value))
                    {
                        if (logger.IsDebugEnabled) logger.DebugFormat("CultureInfo: Changing LCID: '{0}' -> '{1}'", LCID, value);
                        inner = new System.Globalization.CultureInfo(value);
                        onPropertyChanged();
                    }
                }
            }

            void onPropertyChanged()
            {
                if (PropertyChanged != null)
                {
                    foreach (string name in propertyNames)
                    {
                        PropertyChanged(this, new PropertyChangedEventArgs(name));
                    }
                }
            }
            static readonly string[] propertyNames = { "Name", "DisplayName", "EnglishName", "NativeName", "LCID", };

            public string DisplayName { get { return (inner == null) ? "" : inner.DisplayName; } }
            public string EnglishName { get { return (inner == null) ? "" : inner.EnglishName; } }
            public string NativeName { get { return (inner == null) ? "" : inner.NativeName; } }

            System.Globalization.CultureInfo inner = null;

            #region Implementation of INotifyPropertyChanged
            public event PropertyChangedEventHandler PropertyChanged;
            #endregion

            #region Implemetation of IFormattable
            public string ToString(string format, IFormatProvider formatProvider)
            {
                if (!string.IsNullOrEmpty(format))
                {
                    switch (format[0])
                    {
                        case 'L': return LCID.ToString(format.Substring(1), formatProvider);
                        case 'D': return DisplayName;
                        case 'E': return EnglishName;
                        case 'N': return NativeName;
                    }
                }
                return Name;
            }
            #endregion
        }

        private void onShowButtonClick(object sender, RoutedEventArgs e)
        {
            new DialogBox(DialogBoxType).Show();
        }
    }
}
