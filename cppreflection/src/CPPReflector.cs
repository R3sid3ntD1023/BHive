using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace Reflection
{ // FileParser class to parse C++ header files and generate RTTR code

    class FileParser
    {
        public Dictionary<string, List<BaseClass>> ReflectedTypes { get; private set; } // The types that are reflected in the file

        private string _Regex = @"DECLARE_(?:CLASS|STRUCT|ENUM)\((?<meta>(?:.+|))\)\s*(?<type>class|struct|enum(?:\sclass)?)(?<name>.+?)(?:\s|:(?:.*?)(?<base>[^\s].*))+\{(?<content>[^}]*)\}";


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
                // Parse the file

                string content = File.ReadAllText(file);
                Console.WriteLine($"Parsing {file}");

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

        public void Generate(string directory)
        {
            if (!Directory.Exists(directory))
            {
                Directory.CreateDirectory(directory);
            }

            foreach (var kvp in ReflectedTypes)
            {
                string fileName = Path.GetFileNameWithoutExtension(kvp.Key) + ".generated.cpp";
                string filePath = Path.Combine(directory, fileName);

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
    }
}