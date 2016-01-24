using System;
using System.Windows.Forms;
using EngineNET;
using System.Threading;
using System.Diagnostics;

namespace EngineEditor
{
    class Game
    {
        public void Start()
        {
            Debug.WriteLine("Engine Resources created.");
        }

        public void Update()
        {

        }

        public void Draw()
        {

        }

        public void Destroy()
        {
            Debug.WriteLine("Engine destroying.");
        }
    }

    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Debug.WriteLine("Engine Starting.");

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            Game game = new Game();
            StartFunc start = new StartFunc(game.Start);
            UpdateFunc update = new UpdateFunc(game.Update);
            DrawFunc draw = new DrawFunc(game.Draw);
            DestroyFunc destroy = new DestroyFunc(game.Destroy);

            DX12Renderer renderer = (DX12Renderer)Renderer.CreateRenderer();
            Core.Initialise(1024, 768, true, start, update, draw, destroy);

            while (Core.Running())
            {
                if (Core.Update())
                {
                    break;
                }

                renderer.SetWindowTitle(new EngineNET.String("Engine in .NET glory"));

                Thread.Sleep(1);
            }

            Core.Destroy();

            Debug.WriteLine("Engine destroyed.");
        }
    }
}
