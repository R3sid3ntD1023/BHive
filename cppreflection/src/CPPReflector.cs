using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace Reflection
{ // FileParser class to parse C++ header files and generate RTTR code

    // FileParser class to parse C++ header files and generate RTTR code
    class FileParser
    {
        public Dictionary<string, List<BaseClass>> ReflectedTypes { get; private set; }
        private Dictionary<string, DateTime> _FileTimeStamps = new Dictionary<string, DateTime>();
        private string _OutputDirectory;
        private string _OutputFile => Path.Combine(_OutputDirectory, "timestamps.txt");
        private string _Regex = @"DECLARE_(?:CLASS|STRUCT|ENUM)\((?<meta>(?:.+|))\)\s*(?<type>class|struct|enum(?:\sclass)?)(?<name>.+?)(?:\s|:(?:.*?)(?<base>[^\s].*))+\{(?<content>((?>[^{}]+|{(?<c>)|}(?<-c>))*(?(c)(?!))))\}";

        public FileParser(string outputDirectory)
        {
            _OutputDirectory = outputDirectory;
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

            string[] files = Directory.GetFiles(directory, "*.h", SearchOption.AllDirectories);

            if (ReflectedTypes == null)
                ReflectedTypes = new Dictionary<string, List<BaseClass>>();

            foreach (string file in files)
            {
                // Parse the file only if it has been modified
                if (!HasFileChanged(file))
                {
                    continue;
                }

                Console.WriteLine($"Parsing {file}");
                _FileTimeStamps[file] = GetFileTimeStamp(file);

                string content = File.ReadAllText(file);

                // Regex to match all namespaces and their content, including nested namespaces
                Regex namespaceRegex = new Regex(@"namespace\s+(?<name>\w+)\s*{(?<content>((?>[^{}]+|{(?<c>)|}(?<-c>))*(?(c)(?!))))}", RegexOptions.Singleline);
                MatchCollection namespaceMatches = namespaceRegex.Matches(content);

                if (namespaceMatches.Count > 1)
                {
                    Console.WriteLine($"File {file} contains more than one namespace.");
                }

                if (namespaceMatches.Count == 0)
                {
                    // No namespace, parse the entire file content
                    ParseContent(file, content, string.Empty);
                }
                else
                {
                    // Parse content within each namespace
                    foreach (Match namespaceMatch in namespaceMatches)
                    {
                        string namespaceName = namespaceMatch.Groups["name"].Value;
                        string namespaceContent = namespaceMatch.Groups["content"].Value;
                        ParseContent(file, namespaceContent, namespaceName);
                    }
                }
            }
        }

        private void ParseContent(string file, string content, string namespaceName)
        {
            Regex regex = new Regex(_Regex);
            MatchCollection collection = regex.Matches(content);

            foreach (Match match in collection)
            {
                BaseClass _class = null;

                switch (match.Groups["type"].Value)
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

        public void Generate()
        {
            if (!Directory.Exists(_OutputDirectory))
            {
                Directory.CreateDirectory(_OutputDirectory);
            }

            foreach (var kvp in ReflectedTypes)
            {
                List<string> namespaces = new List<string>();
                

                string fileName = Path.GetFileNameWithoutExtension(kvp.Key) + ".generated.cpp";
                string filePath = Path.Combine(_OutputDirectory, fileName);

                using (StreamWriter writer = new StreamWriter(filePath))
                {
                    string header = $"#include \"{kvp.Key}\"\r\n";
                    header += "#include <rttr/registration.h>\r\n";

                    foreach (var type in kvp.Value)
                    {
                        if (!namespaces.Contains(type.Namespace))
                        {
                            namespaces.Add(type.Namespace);
                            header += $"using namespace {type.Namespace};\r\n";
                        }
                    }

                   
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

        private bool HasFileChanged(string file)
        {
            DateTime currentTimestamp = GetFileTimeStamp(file);
            if (_FileTimeStamps.ContainsKey(file))
            {
                DateTime storedTimestamp = _FileTimeStamps[file];
                // Compare timestamps with a tolerance of 1 second
                bool hasChanged = Math.Abs((currentTimestamp - storedTimestamp).TotalSeconds) > 1;
                return hasChanged;
            }
            return true;
        }
    }
}