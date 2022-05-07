//#include <iostream>
//#include <fstream>
//#include "../MaterialObject.h"
//
//using namespace std;
//using namespace nlohmann;
//using namespace MaterialSerialization;
//
//int main()
//{
//	ifstream file;
//	file.open(L"Assets\\Materials\\Mat.json", ios::in);
//	json j;
//	if (file.is_open())
//	{
//		file >> j;
//	}
//	file.close();
//	auto object = j.get<MaterialObject>();
//
//	json j1 = object;
//	cout << j1 << endl;
//	ofstream outFile(L"Assets\\Materials\\Mat.json", ios::out);
//	if (outFile.is_open())
//	{
//		outFile << j1 << endl;
//	}
//	outFile.close();
//
//	system("pause");
//
//	return 0;
//}