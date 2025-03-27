using System.Text.RegularExpressions;

namespace Reflection
{
    //Method class to represent a method in C++
    class Method : BaseClass
    {
        public string ReturnType { get; private set; }
        public string Args { get; private set; }
        public bool IsConst { get; private set; }
        public Class Owner { get; private set; }

        public Method(Class owner)
        {
            Owner = owner;
            Namespace = owner.Namespace;
        }

        public static string _Regex = @"DECLARE_FUNCTION\((?<meta>[^)]*)\)\s*(?:virtual\s)?(?<type>[^\s]*)?\s(?<name>[^(|]+)?\((?<args>[^)]*)\)\s*(const)?";

        override public void Parse(Match match)
        {
            base.Parse(match);

            ReturnType = match.Groups["type"].Value.Trim();
            Args = match.Groups["args"].Value.Trim();
            IsConst = match.Groups[3].Success;
        }

        override public string GenerateRTTR()
        {
            string rttrdefinition = $".method(\"{Name}\", &{Namespace}::{Owner.Name}::{Name})";

            if (Metadatas.Count > 0)
            {
                rttrdefinition += "\n\t\t(";
                rttrdefinition += string.Join(",", Metadatas.ConvertAll(meta => meta.GenerateRTTR()));
                rttrdefinition += ")";
            }
            return rttrdefinition;
        }
    }
}