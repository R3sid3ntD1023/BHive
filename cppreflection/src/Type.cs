using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Reflection
{

    // MetaData class to represent metadata in C++
    abstract class BaseClass
    {
        public string Name { get; set; }
        public string Namespace { get; set; } // The namespace of the class
        public List<MetaData> Metadatas { get; set; } = new List<MetaData>();// The metadata of the class
        public string FullName => $"{Namespace}::{Name}"; // The full name of the class

        public virtual void Parse(Match match)
        {
            Name = match.Groups["name"].Value.Trim();

            string[] metadata = match.Groups["meta"].Value.Split([',']);
            if (metadata.Length > 0)
            {
                foreach (string meta in metadata)
                {

                    if (!string.IsNullOrEmpty(meta))
                    {
                        string[] strings = meta.Split('=');
                        MetaData metaData = new(strings[0].Trim(), strings.Length > 1 ? strings[1].Trim() : string.Empty);
                        Metadatas.Add(metaData);
                        Console.WriteLine($"Parsed metadata: {metaData.Key} = {metaData.Value}");
                    }
                }
            }

        }

        public abstract string GenerateRTTR();
    }
}