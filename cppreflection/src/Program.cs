using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;

class Program
{
    static void Main(string[] args)
    {
        if (args.Length < 3)
        {
            Console.WriteLine("Usage: CPPReflector.exe --input <input_directory> --output-dir <output_directory> --include <include_directory>");
            return;
        }

        string inputDir = null;
        string outputDir = null;
        List<string> includeDirs = new List<string>();

        for (int i = 0; i < args.Length; i++)
        {
            if (args[i] == "--input" && i + 1 < args.Length)
            {
                inputDir = args[i + 1];
                i++;
            }
            else if (args[i] == "--output" && i + 1 < args.Length)
            {
                outputDir = args[i + 1];
                i++;
            }
            else if (args[i] == "--include" && i + 1 < args.Length)
            {
                includeDirs.Add(args[i + 1]);
                i++;
            }
        }

        if (inputDir == null || outputDir == null)
        {
            Console.WriteLine("Missing required arguments.");
            return;
        }

        if (!Directory.Exists(inputDir))
        {
            Console.WriteLine($"The input directory {inputDir} does not exist.");
            return;
        }

        if (!Directory.Exists(outputDir))
        {
            Console.WriteLine($"The output directory {outputDir} does not exist.");
            return;
        }

        Reflection.FileParser parser = new Reflection.FileParser();

        Console.WriteLine($"Parsing files in {inputDir}...");
        parser.Parse(inputDir);
        foreach (var include in includeDirs)
        {
            parser.Parse(include);
        }

        // Print the reflected types
        parser.PrintReflectedTypes();

        Console.WriteLine($"Generating files in {outputDir}...");
        parser.Generate(outputDir);

        Console.ReadLine();
    }
}
