using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Reflection
{
    // Enum class to represent an enum in C++
    class Enum : BaseClass
    {
        public string UnderlyingType { get; private set; } // The underlying type of the enum
        public string EnumType { get; private set; } // The type of the enum
        public List<string> EnumValues { get; private set; } = new List<string>(); // The values of the enum

        public override void Parse(Match match)
        {
            base.Parse(match);


            UnderlyingType = match.Groups["base"].Value;

            string[] enumValues = match.Groups["content"].Value.Split(',');
            foreach (string value in enumValues)
            {
                string value_name = value.Split('=')[0].Trim();
                EnumValues.Add(value_name);
            }

        }

        public override string ToString()
        {
            return $"Enum: {Name}, NameSpace: {Namespace} , UnderlyingType: {UnderlyingType}, MetaData: {string.Join(", ", Metadatas)}, Values: {string.Join(", ", EnumValues)}";
        }

        public override string GenerateRTTR()
        {

            // Generate the RTTR code for the enum
            string rttrDefinition = $"\trttr::registration::enumeration<{Namespace}::{Name}>(\"{Name}\")";

            rttrDefinition += "\n\t(\n";
            if (EnumValues.Count > 0 || Metadatas.Count > 0)
            {
               
                rttrDefinition += string.Join(",\n", EnumValues.ConvertAll(value => $"rttr::value(\"{value}\", {Namespace}::{Name}::{value})"));
              
            }

            if (Metadatas.Count > 0)
            {
                if (EnumValues.Count > 0)
                {
                    rttrDefinition += "\t\t,\n";
                }
                rttrDefinition += string.Join(",\n", Metadatas.ConvertAll(meta => meta.GenerateRTTR()));
            }

            rttrDefinition += "\n\t\t)";
            rttrDefinition += ";\n";
            return rttrDefinition;
        }
    }
}