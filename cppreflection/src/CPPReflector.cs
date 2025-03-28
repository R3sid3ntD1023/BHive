using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace Reflection
{ // FileParser class to parse C++ header files and generate RTTR code

    class FileParser
    {
        public Dictionary<string, List<BaseClass>> ReflectedTypes { get; private set; } // The types that are reflected in the file
        private Dictionary<string, DateTime> _FileTimeStamps = new Dictionary<string, DateTime>(); // The timestamps of the files
        private string _OutputDirectory; // The output directory
        private string _OutputFile => Path.Combine(_OutputDirectory, "timestamps.txt"); // The output file
        private string _Regex = @"DECLARE_(?:CLASS|STRUCT|ENUM)\((?<meta>(?:.+|))\)\s*(?<type>class|struct|enum(?:\sclass)?)(?<name>.+?)(?:\s|:(?:.*?)(?<base>[^\s].*))+\{(?<content>[^}]*)\}";

        public FileParser(string ouputDirectory)
        {
            _OutputDirectory = ouputDirectory;
            LoadTimeStamps();
        }

        ~FileParser()
        {
            SaveTimeStamps();
        }

        public void Parse(string directory)
        {

            if (!Directory.Exists(directory))
            {
                throw new DirectoryNotFoundException($"The directory {directory} does not exist.");
            }

            string[] files = System.IO.Directory.GetFiles(directory, "*.h", SearchOption.AllDirectories);

            if(ReflectedTypes == null)
                ReflectedTypes = new Dictionary<string, List<BaseClass>>(); // Initialize the dictionary
 
            foreach (string file in files)
            {
                // Parse the file only if it has been modified
                DateTime timestamp = GetFileTimeStamp(file);
                if (_FileTimeStamps.ContainsKey(file))
                {
                    Console.WriteLine($"Checking {file} for changes");
                    if (timestamp == _FileTimeStamps[file])
                    {
                        continue;
                    }
                }

                Console.WriteLine($"Parsing {file}");
                _FileTimeStamps[file] = timestamp;

                string content = File.ReadAllText(file);

                Regex namespaceRegex = new Regex(@"namespace\s+(\w+)\s*{");
                Match namespaceMatch = namespaceRegex.Match(content);
                string namespaceName = namespaceMatch.Success ? namespaceMatch.Groups[1].Value : string.Empty;
                // Regex to match the macro

                Regex regex = new Regex(_Regex);
                MatchCollection collection = regex.Matches(content);


                foreach (Match match in collection)
                {
                    BaseClass _class = null;

                    switch (match.Groups[2].Value)
                    {
                        case "class":
                        case "struct":
                            _class = new Class();
                            break;

                        case "enum":
                        case "enum class":
                            _class = new Enum();
                            break;
                    }

                    _class.Namespace = namespaceName;
                    _class.Parse(match);

                    if (!ReflectedTypes.ContainsKey(file))
                    {
                        ReflectedTypes[file] = new List<BaseClass>();
                    }
                    ReflectedTypes[file].Add(_class);
                }
            }
        }

        public void PrintReflectedTypes()
        {
            foreach (var kvp in ReflectedTypes)
            {
                Console.WriteLine($"File: {kvp.Key}");
                foreach (var type in kvp.Value)
                {
                    Console.WriteLine(type.ToString());
                }
            }
        }

        public void Generate()
        {
            if (!Directory.Exists(_OutputDirectory))
            {
                Directory.CreateDirectory(_OutputDirectory);
            }

            foreach (var kvp in ReflectedTypes)
            {
                string fileName = Path.GetFileNameWithoutExtension(kvp.Key) + ".generated.cpp";
                string filePath = Path.Combine(_OutputDirectory, fileName);

                using (StreamWriter writer = new StreamWriter(filePath))
                {
                    string header = $"#include \"{kvp.Key}\"\r\n";
                    header += "#include <rttr/registration.h>\r\n";
                    writer.WriteLine(header);
                    writer.WriteLine("RTTR_REGISTRATION\r\n{");

                    foreach (var type in kvp.Value)
                    {
                        string content = type.GenerateRTTR().Replace("\n", "\r\n\t");
                        writer.WriteLine(content);
                    }

                    writer.WriteLine("\r\n}");

                    Console.WriteLine($"Generated {filePath}");
                }
            }
        }

        private void LoadTimeStamps()
        {
            Console.WriteLine("Loading timestamps");

            if (File.Exists(_OutputFile))
            {
                string[] lines = File.ReadAllLines(_OutputFile);
                foreach (string line in lines)
                {
                    string[] parts = line.Split('=');
                    _FileTimeStamps[parts[0]] = DateTime.Parse(parts[1]);
                }
            }
        }

        private void SaveTimeStamps()
        {
            Console.WriteLine("Saving timestamps");

            using (StreamWriter writer = new StreamWriter(_OutputFile))
            {
                foreach (var kvp in _FileTimeStamps)
                {
                    writer.WriteLine($"{kvp.Key}={kvp.Value}");
                }
            }
        }

        private DateTime GetFileTimeStamp(string file)
        {
            return File.GetLastWriteTime(file);
        }
    }
}