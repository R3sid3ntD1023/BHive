using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Linq;

namespace Reflection
{
    class Constructor : BaseClass
    {
        private Class OwningClass { get; set; }
        private string ReturnType { get; set; }
        private List<string> Args { get; set; } = new List<string>();
        private bool IsMethod => Name != OwningClass.Name;

        public Constructor(Class @class)
        {
            OwningClass = @class;
        }

        public static string _Regex = @"DECLARE_CONSTRUCTOR\((?<meta>[^)]*)\)\s*?(?<type>[^\s]*)?\s(?<name>[^(|]+)?\((?<args>[^)]*)\)";

        public override void Parse(Match match)
        {
            base.Parse(match);

            ReturnType = match.Groups["type"].Value.Trim();
            Args = match.Groups["args"].Value.Trim().Split(',').ToList();
        }

        public override string GenerateRTTR()
        {
            string rttrdefinition = ".constructor<";
            rttrdefinition += string.Join(", ", Args);
            rttrdefinition += ">("; 

            if(IsMethod)
            {
                rttrdefinition += $"&{OwningClass.FullName}::{Name}";
            }
            rttrdefinition += ")";
            return rttrdefinition;
        }
    }
}