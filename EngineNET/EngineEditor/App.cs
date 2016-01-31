using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using EngineNET;

namespace EngineEditor
{
    public class App : Application
    {
        private MainWindow _window;
        private DX12Renderer _renderer;

        [STAThread]
        public static void Main()
        {
            App app = new App();
            app.InitializeComponent();
            app.Run();
        }

        private void InitializeComponent()
        {
            _window = new MainWindow();

            _window.Closed += (sender, args) =>
            {
                Core.Exit();
            };

            _window.btnBlue.Click += (sender, args) => _renderer.SetClearColour(Colour.Blue);
            _window.btnGreen.Click += (sender, args) => _renderer.SetClearColour(Colour.Green);
            _window.btnRed.Click += (sender, args) => _renderer.SetClearColour(Colour.Red);

            _window.Show();

            Game game = new Game();
            StartFunc start = game.Start;
            UpdateFunc update = game.Update;
            DrawFunc draw = game.Draw;
            DestroyFunc destroy = game.Destroy;

            _renderer = (DX12Renderer)Renderer.CreateRenderer();
            _renderer.SetWindowTitle("Engine in .NET glory");
            Core.Initialise(1024, 768, true, start, update, draw, destroy, _window.pbView.Handle);
            _window.HookWndProc();

            CompositionTarget.Rendering += EngineUpdate;
        }

        private void EngineUpdate(object sender, EventArgs eventArgs)
        {
            if (Core.Update() || !Core.Running())
            {
                Core.Destroy();
                _window.Close();
            }
        }

        public static IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            Renderer.ProcessWindowMessage(hwnd, msg, wParam, lParam);           
            return IntPtr.Zero;
        }
    }

    internal class Game
    {
        private RenderObject _pTriangle;
        private Texture _pTexture;
        private Material _pMaterial;
        private RenderObject _pTriangle2;
        private Texture _pTexture2;
        private Material _pMaterial2;

        private Font _pFont;
        private Text _pText;
        private Text _pText2;

        private float _timer;
        private bool _done;

        private VertexBufferInstance _vertexBuffer;
        private VertexBufferInstance _vertexBuffer2;
        private IndexBufferInstance _indexBuffer;

        private List<Vertex> _triangleVertices;
        private List<Vertex> _vertices2;
        private List<int> _indices;

        public void Start()
        {
            // Create an example triangle object.
            _triangleVertices = new List<Vertex>(6)
            {
                new Vertex(new Vector3(-0.25f, 0.25f, 0.0f), Colour.White, new Vector2(0.0f, 0.0f)),
                new Vertex(new Vector3(0.25f, -0.25f, 0.0f), Colour.White, new Vector2(1.0f, 64.0f)),
                new Vertex(new Vector3(-0.25f, -0.25f, 0.0f), Colour.White, new Vector2(0.0f, 64.0f)),

                new Vertex(new Vector3(-0.25f, 0.25f, 0.0f), Colour.White, new Vector2(0.0f, 0.0f)),
                new Vertex(new Vector3(0.25f, 0.25f, 0.0f), Colour.White, new Vector2(1.0f, 0.0f)),
                new Vertex(new Vector3(0.25f, -0.25f, 0.0f), Colour.White, new Vector2(1.0f, 64.0f))
            };

            _vertexBuffer = ResourceFactory.CreateVertexBufferInstance();
            _vertexBuffer.SetVertices(_triangleVertices);

            _pTexture = ResourceFactory.CreateTexture();
            _pTexture.Load(Helpers.GetRelativePath("Textures\\font.dds"));

            _pMaterial = ResourceFactory.CreateMaterial();
            _pMaterial.SetTexture(_pTexture);
            _pMaterial.LoadVertexShader(Helpers.GetRelativePath("Shaders\\DiffuseTexture.hlsl"), "VSMain", "vs_5_1");
            _pMaterial.LoadPixelShader(Helpers.GetRelativePath("Shaders\\DiffuseTexture.hlsl"), "PSMain", "ps_5_1");
            _pMaterial.Finalise(false);

            _pTriangle = new RenderObject("triangle");
            _pTriangle.SetVertexBuffer(_vertexBuffer);
            _pTriangle.SetMaterial(_pMaterial);

            // Create an example triangle object.
            _vertices2 = new List<Vertex>(4)
            {
                new Vertex(new Vector3(-0.25f, 0.25f, 0.0f), Colour.White, new Vector2(0.0f, 0.0f)),
                new Vertex(new Vector3(0.25f, -0.25f, 0.0f), Colour.White, new Vector2(1.0f, 1.0f)),
                new Vertex(new Vector3(0.25f, 0.25f, 0.0f), Colour.White, new Vector2(1.0f, 0.0f)),
                new Vertex(new Vector3(-0.25f, -0.25f, 0.0f), Colour.White, new Vector2(0.0f, 1.0f))
            };

            _indices = new List<int>(6)
            {
                1, 0, 2,
                0, 1, 3
            };

            _vertexBuffer2 = ResourceFactory.CreateVertexBufferInstance();
            _vertexBuffer2.SetVertices(_vertices2);

            _indexBuffer = ResourceFactory.CreateIndexBufferInstance();
            _indexBuffer.SetIndices(_indices);

            _pTexture2 = ResourceFactory.CreateTexture();
            _pTexture2.Load(Helpers.GetRelativePath("Textures\\test2.png"));

            _pMaterial2 = ResourceFactory.CreateMaterial();
            _pMaterial2.SetTexture(_pTexture2);
            _pMaterial2.LoadVertexShader(Helpers.GetRelativePath("Shaders\\DiffuseTexture.hlsl"), "VSMain", "vs_5_1");
            _pMaterial2.LoadPixelShader(Helpers.GetRelativePath("Shaders\\DiffuseTexture.hlsl"), "PSMain", "ps_5_1");
            _pMaterial2.Finalise(false);

            _pTriangle2 = new RenderObject("triangle2");
            _pTriangle2.SetIndexBuffer(_indexBuffer);
            _pTriangle2.SetVertexBuffer(_vertexBuffer2);
            _pTriangle2.SetMaterial(_pMaterial2);

            _pFont = FontManager.LoadFont("Myriad", Helpers.GetRelativePath("Textures\\myriad.dds"), Helpers.GetRelativePath("Textures\\myriad.txt"));
            _pText = new Text("text", _pFont);
            _pText.SetText("test");
            _pText.SetColour(Colour.Yellow);
            _pText.Transform.Position = new Vector3(0.0f, 0.0f, 0.0f);
            _pText2 = new Text("text2", _pFont);
            _pText2.SetText("Hello world!");
            _pText2.Transform.Position = new Vector3(0.0f, 20.0f, 0.0f);
            _pText2.EnableWorldSpace(true);
        }

        public void Update()
        {
            _timer += Time.DeltaTime();

            if (!_done && _timer > 2.0f)
            {
                _done = true;
            }

            if (_pTriangle != null)
            {
                _pTriangle.Transform.Rotate(45.0f * Time.DeltaTime(), 35.0f * Time.DeltaTime(), 25.0f * Time.DeltaTime());
            }

            if (_pTriangle2 != null)
            {
                _pTriangle2.Transform.Position = new Vector3(Mathf.Cos(_timer), Mathf.Sin(_timer), 0.0f);
            }

            if (_pText != null)
            {
                string text = string.Format("Framerate: {0} FPS", 1f / Time.DeltaTime());
                _pText.SetText(text);
            }

            if (_pText2 != null)
            {
                _pText2.Transform.Position = new Vector3(Mathf.Cos(-_timer + Mathf.Sin(_timer)) / 2, Mathf.Sin(-_timer) / 2, 0.0f);
                _pText2.Transform.Rotate(0.0f, 0.0f, 100.0f * Time.DeltaTime());
                _pText2.Transform.Scale = new Vector3(1.0f + Mathf.Cos(-_timer) / 2, 1.0f + Mathf.Cos(-_timer) / 2, 1.0f);
            }
        }

        public void Draw()
        {
            _pTriangle?.Draw();
            _pTriangle2?.Draw();
            _pText?.Draw();
            _pText2?.Draw();
        }

        public void Destroy()
        {
            _pTriangle?.Dispose();
            _pTriangle2?.Dispose();
            _pText?.Dispose();
            _pText2?.Dispose();
            _pFont?.Dispose();
            _pMaterial?.Dispose();
            _pMaterial2?.Dispose();
            _pTexture?.Dispose();
            _pTexture2?.Dispose();
        }
    }
}
