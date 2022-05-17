#pragma once
#include <json.hpp>
#include <string>
#include <unordered_map>

#include "ShaderType.h"

namespace MaterialSerialization
{
	using namespace std;
	using namespace nlohmann;

	struct ResourceID
	{
		int FileID{};
		string GUID;
		int Type{};
	};

	struct ShaderProperty
	{
		unordered_map<string, ResourceID> m_Textures{};
		unordered_map<string, int> m_Ints{};
		unordered_map<string, float> m_Floats{};
		unordered_map<string, float[4]> m_Vectors{};
	};

	struct ShadersObject
	{
		string m_Name;
	};

	struct MaterialObject
	{
		string m_Name;
		ShadersObject m_ShaderObject;
		ShaderProperty m_Properties;
	};

	inline void to_json(json& j, const ResourceID& p)
	{
		j = json
		{
			{ "FileID", p.FileID },
			{ "GUID", p.GUID },
			{ "Type", p.Type }
		};
	}

	inline void to_json(json& j, const MaterialObject& p)
	{
		j = json
		{
			{"m_Name", p.m_Name},
			{
				"m_Shader",
				{
					{"m_Name", p.m_ShaderObject.m_Name}
				}
			},
			{
				"m_Properties",
				{
					{"m_Textures", p.m_Properties.m_Textures},
					{"m_Ints", p.m_Properties.m_Ints},
					{"m_Floats", p.m_Properties.m_Floats},
					{"m_Vectors", p.m_Properties.m_Vectors}
				}
			}
		};
	}

	inline void from_json(const json& j, ResourceID& p)
	{
		const int& FileID = j["FileID"];
		const string& GUID = j["GUID"];
		const int& Type = j["Type"];
		p.FileID = FileID;
		p.GUID = GUID;
		p.Type = Type;
	}

	inline void from_json(const json& j, MaterialObject& p)
	{
		{
			const string& name = j["m_Name"];
			p.m_Name = name;
		}
		{
			const auto& shader = j["m_Shader"];
			p.m_ShaderObject.m_Name = shader["m_Name"];
		}
		{
			const auto& properties = j["m_Properties"];
			const auto& textures = properties["m_Textures"];
			const auto& ints = properties["m_Ints"];
			const auto& floats = properties["m_Floats"];
			const auto& vectors = properties["m_Vectors"];
			for (json::const_iterator it = textures.cbegin(); it != textures.cend(); ++it)
			{
				string key = it.key();
				const auto& tex = it.value();
				p.m_Properties.m_Textures[key] = tex;
			}
			for (json::const_iterator it = ints.cbegin(); it != ints.cend(); ++it)
			{
				string key = it.key();
				p.m_Properties.m_Ints[key] = it.value();
			}
			for (json::const_iterator it = floats.cbegin(); it != floats.cend(); ++it)
			{
				string key = it.key();
				p.m_Properties.m_Floats[key] = it.value();
			}
			for (json::const_iterator it = vectors.cbegin(); it != vectors.cend(); ++it)
			{
				string key = it.key();
				const auto& vec = it.value();
				vec[0].get_to(p.m_Properties.m_Vectors[key][0]);
				vec[1].get_to(p.m_Properties.m_Vectors[key][1]);
				vec[2].get_to(p.m_Properties.m_Vectors[key][2]);
				vec[3].get_to(p.m_Properties.m_Vectors[key][3]);
			}
		}
	}
}
