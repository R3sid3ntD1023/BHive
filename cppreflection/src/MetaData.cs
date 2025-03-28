

namespace Reflection
{
	// This class represents metadata associated with a class, method, or property.
	// It contains a key-value pair and provides methods to parse and generate RTTR code.
	// The metadata can be used to provide additional information about the class, method, or property.
	class MetaData
	{
		public string Key { get; set; } // The key of the metadata
		public string Value { get; set; } // The value of the metadata
		public MetaData(string key, string value)
		{
			Key = key;
			Value = value;
		}

		public override string ToString()
		{
			return $"{Key} = {Value}";
		}

		public string GenerateRTTR()
		{
			// Generate the RTTR code for the metadata
			string value = string.IsNullOrEmpty(Value) ? "1" : $"{Value}";
            return $"rttr::metadata({Key}, {value})";
		}
	}

}