// Copyright (c) Kodi Studios 2021.
// Licensed under the MIT license.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Diagnostics;
using System.IO;

namespace OneNoteMidiTest
{
    [TestClass]
    public class OneNoteMidiLaunchTest
    {
        [TestMethod]
        public void DefaultLaunch()
        {
            // Launch OneNoteMidi.exe
            using (Process oneNoteMidiProcess = Process.Start(GetOneNoteMidiFilePath()))
            {
                oneNoteMidiProcess.WaitForExit();

                Assert.AreEqual(0, oneNoteMidiProcess.ExitCode);
            }
        }

        [TestMethod]
        public void GuitarLaunch()
        {
            string args = "-i 24 -p 80";
            // Launch OneNoteMidi.exe
            using (Process oneNoteMidiProcess = Process.Start(GetOneNoteMidiFilePath(), args))
            {
                oneNoteMidiProcess.WaitForExit();

                Assert.AreEqual(0, oneNoteMidiProcess.ExitCode);
            }
        }

        [TestMethod]
        public void GuitarWithOptionsLaunch()
        {
            string args = "-c 1 -i 24 -p 81 -v 120 -l 2000";
            // Launch OneNoteMidi.exe
            using (Process oneNoteMidiProcess = Process.Start(GetOneNoteMidiFilePath(), args))
            {
                oneNoteMidiProcess.WaitForExit();

                Assert.AreEqual(0, oneNoteMidiProcess.ExitCode);
            }
        }

        [TestMethod]
        public void HelpFlagLaunch()
        {
            string args = "-?";
            // Launch OneNoteMidi.exe
            using (Process oneNoteMidiProcess = Process.Start(GetOneNoteMidiFilePath(), args))
            {
                oneNoteMidiProcess.WaitForExit();

                Assert.AreEqual(0, oneNoteMidiProcess.ExitCode);
            }
        }

        [TestMethod]
        public void OutOfLimitFlag()
        {
            string args = "-c 50";
            // Launch OneNoteMidi.exe
            using (Process oneNoteMidiProcess = Process.Start(GetOneNoteMidiFilePath(), args))
            {
                oneNoteMidiProcess.WaitForExit();

                Assert.AreEqual(1, oneNoteMidiProcess.ExitCode);
            }
        }

        [TestMethod]
        public void MissingNumberFlag()
        {
            string args = "-c";
            // Launch OneNoteMidi.exe
            using (Process oneNoteMidiProcess = Process.Start(GetOneNoteMidiFilePath(), args))
            {
                oneNoteMidiProcess.WaitForExit();

                Assert.AreEqual(1, oneNoteMidiProcess.ExitCode);
            }
        }

        string GetOneNoteMidiFilePath()
        {
            // Sample TestContext.TestRunDirectory:
            // C:\Users\naris\Source\Repos\one-note-midi\one-note-midi\OneNoteMidi\TestResults\Deploy_naris 2021-03-28 18_00_44
            // It's weird that TestResults are inside of OneNoteMidi folder, not OneNoteMidiTest...

            string oneNoteMidiProjectPath = Path.GetDirectoryName(
                Path.GetDirectoryName(TestContext.TestRunDirectory));

            string oneNoteMidiFilePath = Path.Combine(oneNoteMidiProjectPath, Flavor, "OneNoteMidi.exe");

            return oneNoteMidiFilePath;
        }

        #region Context Infrastructure
        private TestContext instance;

        public TestContext TestContext
        {
            set { instance = value; }
            get { return instance; }
        }
        #endregion

#if DEBUG
        const string Flavor = "Debug";
#else
        const string Flavor = "Release";
#endif
    }
}
