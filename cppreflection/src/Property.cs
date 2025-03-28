using System.Linq;
using System.Text.RegularExpressions;

namespace Reflection
{
    // Property class to represent a property in C++
    class Property : BaseClass
    {
        public Property(Class parentClass)
        {
            ClassName = parentClass.Name;
            Namespace = parentClass.Namespace;
        }

        private string AccessModifier { get; set; }
        private bool ReadOnly { get; set; } // Indicates if the property is readonly
        private string ClassName { get; set; } // The name of the class that contains the property
        private string Getter { get; set; } // The getter of the property
        private string Setter { get; set; } // The setter of the property
        private string Policy { get; set; } // The policy of the property
        public static string _Regex = @"DECLARE_PROPERTY\((?<meta>[^)]*)\)\s+(?<type>[^\s]*)(?<name>[^\=\{\;]*)";

        public override void Parse(Match match)
        {
            base.Parse(match);

            if (Metadatas.Count > 0)
            {
                Getter = Metadatas.FirstOrDefault(meta => meta.Key == "Getter")?.Value.Trim();
                Setter = Metadatas.FirstOrDefault(meta => meta.Key == "Setter")?.Value.Trim();
                AccessModifier = Metadatas.FirstOrDefault(meta => meta.Key == "Access")?.Value.Trim();
                ReadOnly = Metadatas.Any(meta => meta.Key == "ReadOnly");
                Policy = Metadatas.FirstOrDefault(meta => meta.Key == "Policy")?.Value.Trim();

                Metadatas.RemoveAll(meta => meta.Key == "Getter" || meta.Key == "Setter" || meta.Key == "Access" || meta.Key == "ReadOnly" || meta.Key == "Policy");
            }
        }

        public override string ToString()
        {
            return $"Property: {Name}, NameSpace: {Namespace}, ClassName: {ClassName}, Getter: {Getter}, Setter: {Setter}, MetaData: {string.Join(", ", Metadatas)}";
        }

        public override string GenerateRTTR()
        {
            // Generate the RTTR code for the property
            string parentName = $"&{Namespace}::{ClassName}::";
            string name = !string.IsNullOrEmpty(Getter) && !string.IsNullOrEmpty(Setter) ? $"{parentName}{Getter}, {parentName}{Setter}" : $"{parentName}{Name}";

            string propertyType = !ReadOnly ? ".property" : ".property_readonly";
            string accessor = string.IsNullOrEmpty(AccessModifier) ? string.Empty : $", {AccessModifier}";
            string rttrDefinition = $"{propertyType}(\"{Name}\", {name}{accessor})";

            if (!string.IsNullOrEmpty(Policy))
            {
                rttrDefinition += $"\n({Policy})";
            }

            if (Metadatas.Count > 0)
            {
                rttrDefinition += "\n\t\t(";
                rttrDefinition += string.Join(",", Metadatas.ConvertAll(meta => meta.GenerateRTTR()));
                rttrDefinition += ")";
            }
            return rttrDefinition;
        }
    }

}