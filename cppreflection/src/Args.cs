using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Reflection
{
    class Arg
    {
        public string Type { get; set; }
        public string Name { get; set; }

        public Arg(string type, string name)
        {
            Type = type;
            Name = name;
        }
    }

    class Args
    {
        public List<Arg> Arguments { get; private set; } = new List<Arg>();

        public static string _Regex = @"(?<type>(?:const\s+)?(?:\w+::)*\w+(?:\s*[\*&]+)*)(?:\s+(?<name>\w+))?(?:\s*=\s*[^,]*)?";

        public void Parse(string args)
        {
            Arguments.Clear();
            var argList = SplitArguments(args);

            var argRegex = new Regex(_Regex);

            foreach (var arg in argList)
            {
                var matchArg = argRegex.Match(arg.Trim());
                if (matchArg.Success)
                {
                    var type = matchArg.Groups["type"].Value.Trim();
                    var name = matchArg.Groups["name"].Value.Trim();
                    Arguments.Add(new Arg(type, name));
                }
            }
        }

        private List<string> SplitArguments(string args)
        {
            var result = new List<string>();
            var currentArg = new List<char>();
            var nestedLevel = 0;

            foreach (var c in args)
            {
                if (c == ',' && nestedLevel == 0)
                {
                    result.Add(new string(currentArg.ToArray()));
                    currentArg.Clear();
                }
                else
                {
                    if (c == '(' || c == '{' || c == '[')
                        nestedLevel++;
                    else if (c == ')' || c == '}' || c == ']')
                        nestedLevel--;

                    currentArg.Add(c);
                }
            }

            if (currentArg.Count > 0)
                result.Add(new string(currentArg.ToArray()));

            return result;
        }
    }
    
}
