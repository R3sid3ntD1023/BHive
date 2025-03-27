using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace Reflection
{

    // Class class to represent a class in C++
    class Class : BaseClass
    {
        public List<Method> Methods { get; set; } = new List<Method>(); // The methods of the class
        public List<Property> Properties { get; set; } = new List<Property>(); // The properties of the class

        public override void Parse(Match match)
        {
            base.Parse(match);

            var propertyMatches = Regex.Matches(match.Groups["content"].Value, Property._Regex);
            var methodMatches = Regex.Matches(match.Groups["content"].Value, Method._Regex);

            foreach (Match propertyMatch in propertyMatches)
            {
                Property property = new Property(this);
                property.Parse(propertyMatch);
                Properties.Add(property);
            }

            foreach (Match methodMatch in methodMatches)
            {
                Method method = new Method(this);
                method.Parse(methodMatch);
                Methods.Add(method);
            }
        }

        public override string GenerateRTTR()
        {
            // Generate the RTTR code for the class
            string rttrDefinition = $"rttr::registration::class_<{Namespace}::{Name}>(\"{Name}\")";
            if (Metadatas.Count > 0)
            {
                rttrDefinition += "\n\t\t(";
                rttrDefinition += string.Join(",", Metadatas.ConvertAll(meta => meta.GenerateRTTR()));
                rttrDefinition += ")";
            }
            if (Methods.Count > 0)
            {
                rttrDefinition += string.Join("\n", Methods.ConvertAll(method => method.GenerateRTTR()));
            }
            if (Properties.Count > 0)
            {
                rttrDefinition += string.Join("\n", Properties.ConvertAll(property => property.GenerateRTTR()));
            }

            rttrDefinition += ";";
            return rttrDefinition;
        }
    }
}