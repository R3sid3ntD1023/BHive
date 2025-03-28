using System.Text.RegularExpressions;
using System.Linq;

namespace Reflection
{
    // Method class to represent a method in C++
    class Method : BaseClass
    {
        public string ReturnType { get; set; }
        public Args _args { get; set; } = new Args();
        public bool IsConst { get; set; }
        public bool IsVirtual { get; set; }
        public bool IsStatic { get; set; }
        public Class Owner { get; set; }

        public Method(Class owner)
        {
            Owner = owner;
        }

        public static string _Regex = @"DECLARE_FUNCTION\((?<meta>[^)]*)\)\s*(?<specifiers>(?:virtual\s|static\s)*)?(?<type>[^&]+[^\s]*)?\s(?<name>[^(|]+)?\((?<args>[^)]*)\)\s*(const)?";

        public override void Parse(Match match)
        {
            base.Parse(match);

            ReturnType = match.Groups["type"].Value.Trim();
            var args = match.Groups["args"].Value.Trim();
            IsConst = match.Groups["const"].Success;
            IsVirtual = match.Groups["specifiers"].Value.Contains("virtual");
            IsStatic = match.Groups["specifiers"].Value.Contains("static");

            _args.Parse(args);
        }

        public override string GenerateRTTR()
        {
            string rttrdefinition = $"rttr::select_overload<{ReturnType}(";
            rttrdefinition += string.Join(", ", _args.Arguments.Select(arg => arg.Type));
            rttrdefinition += $")";

            if (IsConst)
            {
                rttrdefinition += " const";
            }

            rttrdefinition += $">(&{Owner.FullName}::{Name})";
            return rttrdefinition;
        }
    }
}