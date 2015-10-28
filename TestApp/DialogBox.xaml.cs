using System;
using System.Windows;

namespace TestApp
{
    public enum DialogType
    {
        Progress, Error
    }

    /// <summary>
    /// Interaction logic for DialogBox.xaml
    /// </summary>
    public partial class DialogBox : Window
    {
        public DialogBox(DialogType type)
        {
            this.DialogType = type;

            InitializeComponent();
        }

        public DialogType DialogType { get; protected set; }

        private void onTestButtonClick(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("onTestButtonClick");
        }
    }
}
