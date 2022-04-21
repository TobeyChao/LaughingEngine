#ifndef TYPELIST_
#define TYPELIST_
#include <tuple>

#pragma region TypeList
using std::tuple;

template<typename ...TS>
struct TypeList {};

template<typename TypeList>
struct TypeListToTuple {};

template<typename ...TS>
struct TypeListToTuple<TypeList<TS...>>
{
	using Type = std::tuple<TS...>;
};

template<typename ...TS>
using TSToTuple_t = typename TypeListToTuple<TypeList<TS...>>::Type;

template<typename TypeList>
using TypeListToTuple_t = typename TypeListToTuple<TypeList>::Type;
#pragma endregion

#pragma region Check
// 判断是不是TypeList
template<typename T>
struct IsTypeList
{
	constexpr static bool Value = std::false_type();
};

template<typename ...T>
struct IsTypeList<TypeList<T...>>
{
	constexpr static bool Value = std::true_type();
};
#pragma endregion

#pragma region Size
//template<typename ...T>
//struct TSSize;
//
//template<>
//struct TSSize<>
//{
//	static constexpr size_t Value = 0;
//};
//
//template<typename This, typename ...Rest>
//struct TSSize<This, Rest...>
//{
//	static constexpr size_t Value = TSSize<Rest...>::Value + 1;
//};

template<typename ...TS>
struct TLLength;

//template<typename ...TS>
//struct TLLength<TypeList<TS...>>
//{
//	static constexpr size_t Value = TSSize<TS...>::Value;
//};

template<typename ...TS>
struct TLLength<TypeList<TS...>>
{
	static constexpr size_t Value = sizeof...(TS);
};

template<typename TypeList>
constexpr size_t Length()
{
	return TLLength<TypeList>::Value;
}

//template<typename ...TS>
//constexpr size_t Length_TS()
//{
//	return TSSize<TS...>::Value;
//}

template<typename ...TS>
constexpr size_t Length_TS()
{
	return sizeof...(TS);
}
#pragma endregion

#pragma region SizeSum
template<typename ...TS>
struct SizeSum;

template<typename T, typename ...TS>
struct SizeSum<TypeList<T, TS...>>
{
	static constexpr size_t Value = sizeof(T) + SizeSum<TypeList<TS...>>::Value;
};

template<>
struct SizeSum<TypeList<>>
{
	static constexpr size_t Value = 0;
};
#pragma endregion


#pragma region Contain
template<typename T, typename...TS>
struct TSContain;

template<typename T, typename U>
struct TSContain<T, U>
{
	static constexpr bool Value = std::is_same_v<T, U>;
};

template<typename T, typename U, typename...Rest>
struct TSContain<T, U, Rest...>
{
	static constexpr bool Value = std::is_same_v<T, U> ? true : TSContain<T, Rest...>::Value;
};

template<typename T, typename TypeList>
struct Contain;

template<typename T, typename ...TS>
struct Contain<T, TypeList<TS...>>
{
	static constexpr bool Value = TSContain<T, TS...>::Value;
};

#pragma endregion

#pragma region IndexOf
template<size_t N, typename T, typename...TS>
struct TIndexOf;

template<size_t N, typename T, typename U>
struct TIndexOf<N, T, U>
{
	static constexpr size_t Value = std::is_same_v<T, U> ? N : -1;
};

template<size_t N, typename T, typename U, typename...Rest>
struct TIndexOf<N, T, U, Rest...>
{
	static constexpr size_t Value = std::is_same_v<T, U> ? N : TIndexOf<N + 1, T, Rest...>::Value;
};

template<typename T, typename...TS>
struct TSIndexOf
{
	static constexpr size_t Value = TIndexOf<0, T, TS...>::Value;
};

//template<typename T, typename...TS>
//struct TIndexOf;
//
//// 最后一个匹配，Index就是0
//template<typename T, typename U>
//struct TIndexOf<T, U>
//{
//	static constexpr size_t Value = std::is_same_v<T, U> ? 0 : -1;
//};
//
//template<typename T, typename U, typename...Rest>
//struct TIndexOf<T, U, Rest...>
//{
//	static constexpr size_t Value = std::is_same_v<T, U> ? 0 : TIndexOf<T, Rest...>::Value + 1;
//};
//
//template<typename T, typename...TS>
//struct TSIndexOf
//{
//	static constexpr size_t Value = TIndexOf<T, TS...>::Value;
//};

template<typename T, typename TypeList>
struct IndexOf;

template<typename T, typename ...TS>
struct IndexOf<T, TypeList<TS...>>
{
	static constexpr size_t Value = TSIndexOf<T, TS...>::Value;
};
#pragma endregion

#pragma region TypeAt
template<size_t Index, typename TypeList>
struct TypeAt;

template<size_t Index, typename This, typename ...Rest>
struct TypeAt<Index, TypeList<This, Rest...>>
{
	using Type = typename TypeAt<Index - 1, TypeList<Rest...>>::Type;
};

template<typename This, typename ...Rest>
struct TypeAt<0, TypeList<This, Rest...>>
{
	using Type = This;
};
#pragma endregion

#pragma region TypeAtFrontOrBack
template<typename TypeList>
struct TypesFront
{
	using Type = typename TypeAt<0, TypeList>::Type;
};

template<typename TypeList>
struct TypesBack
{
	using Type = typename TypeAt<TLLength<TypeList>::Value - 1, TypeList>::Type;
};
#pragma endregion

#pragma region TypeListLink
template<typename T, typename U>
struct TypeListLink
{
	using Type = TypeList<T, U>;
};

template<typename T, typename ...TS>
struct TypeListLink<T, TypeList<TS...>>
{
	using Type = TypeList<T, TS...>;
};

template<typename ...TS, typename T>
struct TypeListLink<TypeList<TS...>, T>
{
	using Type = TypeList<TS..., T>;
};

template<typename ...TS, typename ...US>
struct TypeListLink<TypeList<TS...>, TypeList<US...>>
{
	using Type = TypeList<TS..., US...>;
};
#pragma endregion

#pragma region TypeAssign
template<size_t N, typename T>
struct TypeAssign
{
private:
	using RestTypes = typename TypeAssign<N - 1, T>::Type;
public:
	using Type = typename TypeListLink<T, RestTypes>::Type;
};

template<typename T>
struct TypeAssign<0, T>
{
	using Type = TypeList<>;
};
#pragma endregion

#pragma region TypeInsert
template<size_t Index, typename TypeList, typename U>
struct TypeInsert;

template<size_t Index, typename U, typename T, typename ...TS>
struct TypeInsert<Index, TypeList<T, TS...>, U>
{
private:
	using RestTypes = typename TypeInsert<Index - 1, TypeList<TS...>, U>::Type;
public:
	using Type = typename TypeListLink<T, RestTypes>::Type;
};

template<typename U, typename T, typename ...TS>
struct TypeInsert<0, TypeList<T, TS...>, U>
{
	using Type = typename TypeListLink<U, TypeList<T, TS...>>::Type;
};

template<typename U, typename ...TS>
struct TypeInsert<0, TypeList<TS...>, U>
{
	using Type = typename TypeListLink<U, TypeList<TS...>>::Type;
};

//template<typename U>
//struct TypeInsert<0, TypeList<>, U>
//{
//	using Type = typename TypeListLink<U, TypeList<>>::Type;
//};
#pragma endregion

#pragma region TypeRemove

#pragma endregion

#pragma region ForEach
template <typename Functor, typename T>
constexpr void ForEachImpl(Functor&& f)
{
	f.template operator() < T > ();
}

template <typename TypeList, typename Functor, std::size_t... Is>
constexpr void ForEachCall(Functor&& f, std::index_sequence<Is...>)
{
	(ForEachImpl<Functor, typename TypeAt<Is, TypeList>::Type>(std::forward<Functor>(f)), ...);
}

template <typename TypeList, typename Functor>
constexpr void ForEach(Functor&& f)
{
	ForEachCall<TypeList, Functor>(std::forward<Functor>(f), std::make_index_sequence<Length<TypeList>()>());
}
#pragma endregion
#endif // TYPELIST_
