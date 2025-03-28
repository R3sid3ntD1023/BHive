using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace Reflection
{

    // Class class to represent a class in C++
    class Class : BaseClass
    {
        public List<Method> Methods { get; set; } = new List<Method>();
        public List<Property> Properties { get; set; } = new List<Property>();
        public List<Constructor> Constructors { get; set; } = new List<Constructor>();

        public override void Parse(Match match)
        {
            base.Parse(match);

            // Parse methods
            var methodMatches = Regex.Matches(match.Groups["content"].Value, Method._Regex);
            foreach (Match methodMatch in methodMatches)
            {
                var method = new Method(this);
                method.Parse(methodMatch);
                Methods.Add(method);
            }

            // Parse properties
            var propertyMatches = Regex.Matches(match.Groups["content"].Value, Property._Regex);
            foreach (Match propertyMatch in propertyMatches)
            {
                var property = new Property(this);
                property.Parse(propertyMatch);
                Properties.Add(property);
            }

            // Parse constructors
            var constructorMatches = Regex.Matches(match.Groups["content"].Value, Constructor._Regex);
            foreach (Match constructorMatch in constructorMatches)
            {
                var constructor = new Constructor(this);
                constructor.Parse(constructorMatch);
                Constructors.Add(constructor);
            }
        }

        public override string GenerateRTTR()
        {
            string rttrDefinition = $"rttr::registration::class_<{FullName}>(\"{Name}\")";

            // Generate constructors
            foreach (var constructor in Constructors)
            {
                rttrDefinition += $"\n\t.constructor<{string.Join(", ", constructor._args.Arguments.Select(arg => arg.Type))}>()";
            }

            // Generate methods
            foreach (var method in Methods)
            {
                rttrDefinition += $"\n\t.method(\"{method.Name}\", {method.GenerateRTTR()})";
            }

            // Generate properties
            foreach (var property in Properties)
            {
                rttrDefinition += $"\n\t{property.GenerateRTTR()}";
            }

            return rttrDefinition + ";";
        }
    }
}