using System;
using System.Linq;
using System.Text.RegularExpressions;

namespace Reflection
{
    // Property class to represent a property in C++
    class Property : BaseClass
    {
        public Property(Class parentClass)
        {
            ParentClass = parentClass;
            Namespace = parentClass.Namespace;
        }

        private string AccessModifier { get; set; }
        private bool ReadOnly { get; set; } // Indicates if the property is readonly
        private string Getter { get; set; } // The getter of the property
        private string Setter { get; set; } // The setter of the property
        private string Policy { get; set; } // The policy of the property
        private Class ParentClass { get; set; } // The parent class

        // Updated regex to handle multiline DECLARE_PROPERTY with nested brackets
        public static string _Regex = @"DECLARE_PROPERTY\((?<meta>(?>[^()]+|\((?<c>)|\)(?<-c>))*(?(c)(?!)))\)\s+(?<specifiers>(?:const\s|static\s)*)?(?<type>[^\s]*)\s+(?<name>[^\=\{\;]*)";

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

        private Method FindMethod(string methodName)
        {
            return ParentClass.Methods.FirstOrDefault(m => m.Name == methodName);
        }

        public override string GenerateRTTR()
        {
            // Generate the RTTR code for the property
            string getter = !string.IsNullOrEmpty(Getter) ? FindMethod(Getter)?.GenerateRTTR() : string.Empty;
            string setter = !string.IsNullOrEmpty(Setter) ? FindMethod(Setter)?.GenerateRTTR() : string.Empty;

            if (!string.IsNullOrEmpty(Setter) && string.IsNullOrEmpty(Getter))
            {
                Console.WriteLine($"Error: Setter is set but Getter is not set for property {Name} in class {ParentClass.FullName}.");
                throw new InvalidOperationException("Setter is set but Getter is not set.");
            }

            string propertyType = !ReadOnly ? ".property" : ".property_readonly";
            if (string.IsNullOrEmpty(Getter) && string.IsNullOrEmpty(Setter))
            {
                getter = $"&{ParentClass.FullName}::{Name}";
            }
            else if (string.IsNullOrEmpty(Getter))
            {
                getter = $"&{ParentClass.FullName}::{Name}";
            }
            else if (string.IsNullOrEmpty(Setter))
            {
                propertyType = ".property_readonly";
                setter = string.Empty;
            }

            string accessor = string.IsNullOrEmpty(AccessModifier) ? string.Empty : $", {AccessModifier}";
            string rttrDefinition = $"{propertyType}(\"{Name}\", {getter}";

            if (!string.IsNullOrEmpty(setter))
            {
                rttrDefinition += $", {setter}";
            }

            rttrDefinition += $"{accessor})";

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