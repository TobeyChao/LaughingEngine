//#include <vector>
//
//template <class _Ty>
//_INLINE_VAR constexpr size_t _New_alignof = (_STD max) (alignof(_Ty),
//	static_cast<size_t>(__STDCPP_DEFAULT_NEW_ALIGNMENT__) // TRANSITION, VSO-522105
//	);
//
//template <class _Ty>
//class MyAllocator {
//public:
//	using _From_primary = MyAllocator;
//
//	using value_type = _Ty;
//
//	using size_type = size_t;
//	using difference_type = ptrdiff_t;
//
//	using propagate_on_container_move_assignment = decltype(true);
//
//	constexpr MyAllocator() noexcept {}
//
//	constexpr MyAllocator(const MyAllocator&) noexcept = default;
//	template <class _Other>
//	constexpr MyAllocator(const MyAllocator<_Other>&) noexcept {}
//	_CONSTEXPR20 ~MyAllocator() = default;
//	_CONSTEXPR20 MyAllocator& operator=(const MyAllocator&) = default;
//
//	_CONSTEXPR20 void deallocate(_Ty* const _Ptr, const size_t _Count) {
//		// no overflow check on the following multiply; we assume _Allocate did that check
//		_Deallocate<_New_alignof<_Ty>>(_Ptr, sizeof(_Ty) * _Count);
//	}
//
//	_NODISCARD _CONSTEXPR20 __declspec(allocator) _Ty* allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
//		return static_cast<_Ty*>(sizeof(_Ty) * (_Count));
//	}
//};
//
//template <>
//class MyAllocator<void> {
//public:
//	using value_type = void;
//	using size_type = size_t;
//	using difference_type = ptrdiff_t;
//
//	using propagate_on_container_move_assignment = decltype(true);
//};
//
//template <class _Ty, class _Other>
//_NODISCARD _CONSTEXPR20 bool operator==(const MyAllocator<_Ty>&, const MyAllocator<_Other>&) noexcept {
//	return true;
//}
//
//using namespace std;

//int main()
//{
//	vector<int, allocator<int>> v;
//	v.push_back(2);
//	return 0;
//}