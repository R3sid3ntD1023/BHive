using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Linq;

namespace Reflection
{
    class Constructor : BaseClass
    {
        private Class _owningClass { get; set; }
        private string _returnType { get; set; }
        internal Args _args { get; set; } = new Args();
        private bool IsMethod => Name != _owningClass.Name;

        public Constructor(Class @class)
        {
            _owningClass = @class;
        }

        public static string _Regex = @"DECLARE_CONSTRUCTOR\((?<meta>[^)]*)\)\s*?(?<type>[^\s]*)?\s(?<name>[^(|]+)?\((?<args>[^)]*)\)";

        public override void Parse(Match match)
        {
            base.Parse(match);

            _returnType = match.Groups["type"].Value.Trim();
            var args = match.Groups["args"].Value.Trim();

            _args.Parse(args);
        }

        public override string GenerateRTTR()
        {
            string rttrdefinition = ".constructor<";
            rttrdefinition += string.Join(", ", _args.Arguments.Select(arg => arg.Type));
            rttrdefinition += ">(";

            if (IsMethod)
            {
                rttrdefinition += $"&{_owningClass.FullName}::{Name}";
            }
            rttrdefinition += ")";
            return rttrdefinition;
        }
    }
}