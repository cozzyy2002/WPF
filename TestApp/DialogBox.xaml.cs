using System;
using System.Windows;

namespace TestApp
{
    public enum DialogBoxType
    {
        Progress, Error
    }

    /// <summary>
    /// Interaction logic for DialogBox.xaml
    /// </summary>
    public partial class DialogBox : Window
    {
        public DialogBox(DialogBoxType type)
        {
            this.DialogType = type;

            InitializeComponent();
        }

        public DialogBoxType DialogType { get; protected set; }

        private void onTestButtonClick(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("onTestButtonClick");
        }
    }
}
