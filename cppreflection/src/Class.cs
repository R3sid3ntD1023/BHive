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
        public List<Constructor> Constructors { get; set; } = new List<Constructor>(); // The constructors of the class

        public override void Parse(Match match)
        {
            base.Parse(match);

            var propertyMatches = Regex.Matches(match.Groups["content"].Value, Property._Regex);
            var methodMatches = Regex.Matches(match.Groups["content"].Value, Method._Regex);
            var constructorMatches = Regex.Matches(match.Groups["content"].Value, Constructor._Regex);

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

            foreach (Match constructorMatch in constructorMatches)
            {
                Constructor constructor = new Constructor(this);
                constructor.Parse(constructorMatch);
                Constructors.Add(constructor);
            }
        }

        public override string GenerateRTTR()
        {
            // Generate the RTTR code for the class
            string rttrDefinition = $"\trttr::registration::class_<{Namespace}::{Name}>(\"{Name}\")";
            if (Metadatas.Count > 0)
            {
                rttrDefinition += "\n\t\t(";
                rttrDefinition += string.Join(",", Metadatas.ConvertAll(meta => meta.GenerateRTTR()));
                rttrDefinition += ")";
            }
            if(Constructors.Count > 0)
            {
                rttrDefinition += string.Join("\n\t\t", Constructors.ConvertAll(constructor => constructor.GenerateRTTR()));
            }
            if (Methods.Count > 0)
            {
                rttrDefinition += string.Join("\n\t\t", Methods.ConvertAll(method => method.GenerateRTTR()));
            }
            if (Properties.Count > 0)
            {
                rttrDefinition += string.Join("\n\t\t", Properties.ConvertAll(property => property.GenerateRTTR()));
            }

            rttrDefinition += ";\n";
            return rttrDefinition;
        }
    }
}