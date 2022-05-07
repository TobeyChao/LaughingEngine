#include <Windows.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <locale>
#include <json.hpp>

#include "../StringUtil.h"

using namespace std;
using namespace nlohmann;

namespace ns {

	// a simple struct to model a person
	struct person
	{
		std::wstring name;
		std::wstring address;
		int age = 0;
	};

	void to_json(json& j, const person& p)
	{
		j = json
		{
			{"name", Utility::to_byte_string(p.name)},
			{"address", Utility::to_byte_string(p.address)},
			{"age", p.age}
		};
	}

	void from_json(const json& j, person& p)
	{
		string name;
		string address;
		int age;

		j.at("name").get_to(name);
		j.at("address").get_to(address);
		j.at("age").get_to(age);

		p.name = Utility::to_wide_string(name);
		p.address = Utility::to_wide_string(address);
		p.age = age;
	}
}

bool operator==(ns::person& lhs, ns::person& rhs)
{
	return lhs.age == rhs.age &&
		lhs.address == rhs.address &&
		lhs.name == rhs.name;
}

void TestJson()
{
	locale loc("");
	wcout.imbue(loc);

	{
		locale china("zh_CN.UTF-8");
		wifstream file;
		file.imbue(china);
		file.open(L"Materials\\Person.json", ios::in);
		wostringstream record;
		if (file.is_open())
		{
			record << file.rdbuf();
		}
		file.close();

		wstring str = record.str();
		json j = json::parse(str);

		auto p = j.get<ns::person>();

		locale loc("");
		wcout.imbue(loc);

		wcout << p.name << endl;
		wcout << p.address << endl;
		wcout << p.age << endl;
	}

	{
		locale china("zh_CN.UTF-8");
		wifstream file;
		file.imbue(china);
		file.open(L"Materials\\Person.json", ios::in);
		wostringstream record;
		if (file.is_open())
		{
			record << file.rdbuf();
		}
		file.close();

		wstring str = record.str();
		json j = json::parse(str);

		auto p = j.get<ns::person>();

		locale loc("");
		wcout.imbue(loc);

		wcout << p.name << endl;
		wcout << p.address << endl;
		wcout << p.age << endl;
	}

	wcout << endl;

	{
		ns::person p = { L"ÄáÂ»", L"744 Evergreen Terrace", 60 };

		json j;
		j["name"] = p.name;
		j["address"] = p.address;
		j["age"] = p.age;

		ns::person p1
		{
			j["name"].get<std::wstring>(),
			j["address"].get<std::wstring>(),
			j["age"].get<int>()
		};

		wcout << p.name << endl;
		wcout << p.address << endl;
		wcout << p.age << endl;
	}

	wcout << endl;

	{
		ns::person p{ L"ÄáÂ»", L"744 Evergreen Terrace", 60 };
		json j = p;
		auto p2 = j.get<ns::person>();

		assert(p == p2);

		wcout << p.name << endl;
		wcout << p.address << endl;
		wcout << p.age << endl;
	}
}