using System;
using System.Diagnostics;
using System.IO;

namespace TestApplication {
    class Test {
        static void Main(string[] args) {
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.UseShellExecute = false;
            startInfo.RedirectStandardOutput = true;
            startInfo.RedirectStandardInput = true;

            startInfo.FileName = "atm_cli.exe";
            startInfo.Arguments = "basic";

            Process process = new Process();
            process.StartInfo = startInfo;
            process.Start();

            StreamWriter swriter = process.StandardInput;
            StreamReader sreader = process.StandardOutput;

            string input;

            Console.WriteLine("Enter command:");
            input = Console.ReadLine();

            if(input.Length > 0)
            {
                swriter.WriteLine(input);
                swriter.WriteLine("exit");
                string output = sreader.ReadToEnd();
                Console.WriteLine(output);
            }
            Console.ReadLine();
        }    
    }
}