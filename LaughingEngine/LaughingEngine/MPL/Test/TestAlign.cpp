//#include "../Utils/Align.h"
//#include <iostream>
//
//int main()
//{
//	using namespace Math;
//
//	// 0 1 2 0 3 4 5 6 7 8 9 a b c d e f
//	// 255 0x00ff
//	int x = 1502;
//	int x2 = 1280;
//	int x3 = 64;
//	std::cout << Math::AlignUpWithMask(x, 255) << std::endl;				//1536
//	std::cout << Math::AlignUp(x, 256) << std::endl;						//1536
//	std::cout << Math::AlignDownWithMask(x, 255) << std::endl;			//1280
//	std::cout << std::boolalpha << IsAligned(x, 256) << std::endl;	//false	
//	std::cout << std::boolalpha << IsAligned(x2, 256) << std::endl;	//true
//	std::cout << std::boolalpha << IsPowerOfTwo(x) << std::endl;	//false
//	std::cout << std::boolalpha << IsPowerOfTwo(x3) << std::endl;	//false
//	std::cout << (int)Log2(9) << std::endl;							//16
//	std::cout << (int)Log2(7) << std::endl;							//3
//	std::cout << AlignPowerOfTwo(9) << std::endl;					//16
//	std::cout << AlignPowerOfTwo(7) << std::endl;					//8
//
//	{
//		//_BitScanForward64
//		// 1234 10011010010
//		unsigned long number = 1234;
//		unsigned long index = 0;
//		std::cout << number << "(10011010010)的二进制表示，1的索引，自低位至高位：";
//		while (number != 0)
//		{
//			_BitScanForward64(&index, number);
//			number &= ~(1 << index);
//			std::cout << index << " ";
//		}
//		std::cout << std::endl;
//	}
//	{
//		//_BitScanReverse64
//		// 1234 10011010010
//		unsigned long number = 1234;
//		unsigned long index = 0;
//		std::cout << number << "(10011010010)的二进制表示，1的索引，自高位至低位：";
//		while (number != 0)
//		{
//			_BitScanReverse64(&index, number);
//			number &= ~(1 << index);
//			std::cout << index << " ";
//		}
//		std::cout << std::endl;
//	}
//
//	return 0;
//}