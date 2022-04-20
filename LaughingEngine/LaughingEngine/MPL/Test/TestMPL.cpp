//#include "../MPL.h"
//
//#include <iostream>
//#include <string>
//
//using namespace std;
//
//struct ObjSizCaculator
//{
//	void Test()
//	{
//		ForEach<TypeList<int, float>>(*this);
//	}
//
//	template<typename _ComType>
//	void operator()()
//	{
//		cout << typeid(_ComType).name() << endl;
//	}
//};
//
//int main()
//{
//	using t1 = TypeList<int, float, bool>;
//	using t2 = TypeList<int, std::string, bool>;
//	using t3 = TypeList<int, double, bool, float>;
//	using t4 = TypeList<int, double>;
//
//	constexpr bool isTypeList1 = IsTypeList<int>::Value;
//	constexpr bool isTypeList2 = IsTypeList<t1>::Value;
//	constexpr size_t count1 = Size<t1>();
//	constexpr size_t count2 = Size_TS<int, float, bool>();
//	constexpr bool isContain1 = Contain<int, TypeList<float, int>>::Value;
//	constexpr bool isContain2 = Contain<int, TypeList<float, bool>>::Value;
//	constexpr size_t index = IndexOf<int, TypeList<bool, float, float, int>>::Value;
//	using type1 = TypeAt<1, t4>::Type;
//	using type2 = TypesFront<t1>::Type;
//	using type3 = TypesBack<t1>::Type;
//	using type4 = TypeListLink<int, float>::Type;
//	using type5 = TypeListLink<int, t1>::Type;
//	using type6 = TypeListLink<t1, float>::Type;
//	using type7 = TypeListLink<t1, t2>::Type;
//	using type8 = TypeAssign<2, t1>::Type;
//
//	int a = 5;
//	if (a == 5)
//	{
//		TypeInsert<1, t1, int>::Type x;
//		cout << "Hello a== 5 addr:" << &x << endl;
//	}
//	else
//	{
//		TypeInsert<2, t1, int>::Type y;
//		cout << "Hello a!= 5 addr:" << &y << endl;
//	}
//	ObjSizCaculator fun;
//	fun.Test();
//	return 0;
//}