using System;
using System.Runtime.InteropServices;

namespace EngineEditor
{
    class RawInput
    {
        [StructLayout(LayoutKind.Sequential)]
        internal struct RawInputDevice
        {
            internal HidUsagePage UsagePage;
            internal HidUsage Usage;
            internal RawInputDeviceFlags Flags;
            internal IntPtr Target;

            public override string ToString()
            {
                return string.Format("{0}/{1}, flags: {2}, target: {3}", UsagePage, Usage, Flags, Target);
            }
        }

        public static void RegisterInput(IntPtr handle)
        {
            RawInputDevice[] rid = new RawInputDevice[2];
            rid[0].UsagePage = HidUsagePage.GENERIC; // 0x01
            rid[0].Usage = HidUsage.Mouse; // 0x02
            rid[0].Flags = RawInputDeviceFlags.INPUTSINK;
            rid[0].Target = handle;

            rid[1].UsagePage = HidUsagePage.GENERIC; // 0x01
            rid[1].Usage = HidUsage.Keyboard; // 0x06
            rid[1].Flags = RawInputDeviceFlags.INPUTSINK;
            rid[1].Target = handle;

            RegisterRawInputDevices(rid, (uint)rid.Length, (uint)Marshal.SizeOf(rid[0]));
        }

        [DllImport("User32.dll")]
        static extern bool RegisterRawInputDevices(RawInputDevice[] pRawInputDevice, uint uiNumDevices, uint cbSize);

        internal enum RawInputDeviceFlags
        {
            /// <summary>No flags.</summary>
            NONE = 0,
            /// <summary>If set, this removes the top level collection from the inclusion list. This tells the operating system to stop reading from a device which matches the top level collection.</summary>
            REMOVE = 0x00000001,
            /// <summary>If set, this specifies the top level collections to exclude when reading a complete usage page. This flag only affects a TLC whose usage page is already specified with PageOnly.</summary>
            EXCLUDE = 0x00000010,
            /// <summary>If set, this specifies all devices whose top level collection is from the specified UsagePage. Note that Usage must be zero. To exclude a particular top level collection, use Exclude.</summary>
            PAGEONLY = 0x00000020,
            /// <summary>If set, this prevents any devices specified by UsagePage or Usage from generating legacy messages. This is only for the mouse and keyboard.</summary>
            NOLEGACY = 0x00000030,
            /// <summary>If set, this enables the caller to receive the input even when the caller is not in the foreground. Note that WindowHandle must be specified.</summary>
            INPUTSINK = 0x00000100,
            /// <summary>If set, the mouse button click does not activate the other window.</summary>
            CAPTUREMOUSE = 0x00000200,
            /// <summary>If set, the application-defined keyboard device hotkeys are not handled. However, the system hotkeys; for example, ALT+TAB and CTRL+ALT+DEL, are still handled. By default, all keyboard hotkeys are handled. NoHotKeys can be specified even if NoLegacy is not specified and WindowHandle is NULL.</summary>
            NOHOTKEYS = 0x00000200,
            /// <summary>If set, application keys are handled.  NoLegacy must be specified.  Keyboard only.</summary>
            APPKEYS = 0x00000400,
            /// If set, this enables the caller to receive input in the background only if the foreground application
            /// does not process it. In other words, if the foreground application is not registered for raw input,
            /// then the background application that is registered will receive the input.
            /// </summary>
            EXINPUTSINK = 0x00001000,
            DEVNOTIFY = 0x00002000
        }

        public enum HidUsagePage : ushort
        {
            /// <summary>Unknown usage page.</summary>
            UNDEFINED = 0x00,
            /// <summary>Generic desktop controls.</summary>
            GENERIC = 0x01,
            /// <summary>Simulation controls.</summary>
            SIMULATION = 0x02,
            /// <summary>Virtual reality controls.</summary>
            VR = 0x03,
            /// <summary>Sports controls.</summary>
            SPORT = 0x04,
            /// <summary>Games controls.</summary>
            GAME = 0x05,
            /// <summary>Keyboard controls.</summary>
            KEYBOARD = 0x07,
        }

        public enum HidUsage : ushort
        {
            /// <summary>Unknown usage.</summary>
            Undefined = 0x00,
            /// <summary>Pointer</summary>
            Pointer = 0x01,
            /// <summary>Mouse</summary>
            Mouse = 0x02,
            /// <summary>Joystick</summary>
            Joystick = 0x04,
            /// <summary>Game Pad</summary>
            Gamepad = 0x05,
            /// <summary>Keyboard</summary>
            Keyboard = 0x06,
            /// <summary>Keypad</summary>
            Keypad = 0x07,
            /// <summary>Muilt-axis Controller</summary>
            SystemControl = 0x80,
            /// <summary>Tablet PC controls</summary>
            Tablet = 0x80,
            /// <summary>Consumer</summary>
            Consumer = 0x0C,
        }
    }
}
