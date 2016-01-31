using System.ComponentModel;
using System.Windows.Interop;

namespace EngineEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        public void HookWndProc()
        {
            WindowInteropHelper helper = new WindowInteropHelper(this);
            HwndSource source = HwndSource.FromHwnd(helper.Handle);

            if (source != null)
            {
                RawInput.RegisterInput(source.Handle);
                source.AddHook(App.WndProc);
            }
        }

        private void MainWindow_OnClosing(object sender, CancelEventArgs e)
        {
            WindowInteropHelper windowHandle = new WindowInteropHelper(this);
            HwndSource source = HwndSource.FromHwnd(windowHandle.Handle);
            source?.RemoveHook(App.WndProc);
        }
    }
}
