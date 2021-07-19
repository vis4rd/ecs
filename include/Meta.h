#pragma once

#include "Util.h"

namespace ecs
{
namespace meta
{
	// ############################################################################################
	// TypeList

	template <typename ...Typepack>
	struct TypeList;

	// ############################################################################################
	// Implementation of TypeListSize.

	template<typename TypeListT>
	struct TypeListSizeImpl;  // dummy struct used just to hold TypeList

	template<typename... Typepack>
	struct TypeListSizeImpl<TypeList<Typepack...>>  // specialization of TypeListSizeImpl
	{
	    static constexpr uint16 Size = sizeof...(Typepack);
	};
	template<typename TypeListT>
	constexpr uint16 TypeListSize = TypeListSizeImpl<TypeListT>::Size;

	// ############################################################################################
	// Implementation of TypeListAt.

	// 1)
	template<uint16 Index, typename TypeListT>
	struct TypeAtImpl;  // dummy struct used just to hold TypeList and call specialization
	// 2)
	template<uint16 Index, typename FirstType, typename... RestOfTypes>
	struct TypeAtImpl<Index, TypeList<FirstType, RestOfTypes...>>  // specialization of TypeAtImpl for N
	{
	    using Type = typename TypeAtImpl<Index - 1, TypeList<RestOfTypes...>>::Type;
	};
	// 3)
	template<typename FirstType, typename... RestOfTypes>
	struct TypeAtImpl<0, TypeList<FirstType, RestOfTypes...>>  // specialization of TypeAtImpl for 0
	{
	    using Type = FirstType;
	};
	// 4)
	template<uint16 Index, typename TypeListT>
	using TypeAt = typename TypeAtImpl<Index, TypeListT>::Type;

	// User calls (4).
	// (4) calls (1).
	// (1) calls (2), because (2) is a specialization of (1) and
	//   TypeList = (through argument deduction) FirstType + ...RestOfTypes
	//   (extraction of the first type from TypeList).
	// (2) loops until calls specialization for 0 which is (3).
	//
	// example:
	// TypeListAt<2, TypeList<T1, T2, T3>> =>
	// TypeListAtImpl<2, TypeList<T1, T2, T3>>::Type =>
	// TypeListAtImpl<1, TypeList<T2, T3>>::Type =>
	// TypeListAtImpl<0, TypeList<T3>>::Type =>
	// T3
	//   calls: user -> (4) -> (1) -> (2) -> (1) -> (2) -> (1) -> (3)
	//
	// example2:
	// TypeListAt<1, TypeList<T1, T2, T3>> =>
	// TypeListAtImpl<1, TypeList<T1, T2, T3>>::Type =>
	// TypeListAtImpl<0, TypeList<T2, T3>>::Type =>
	// T2
	//   calls: user -> (4) -> (1) -> (2) -> (1) -> (3)

	// ############################################################################################
	// Implementation of IndexOf

	template <uint16 Index, typename Target, typename TypeListT>
	struct IndexOfImpl;

	template <uint16 Index, typename Target, typename FirstType, typename... RestOfTypes>
	struct IndexOfImpl<Index, Target, TypeList<FirstType, RestOfTypes...>>
	{
		static constexpr uint16 ID = ((std::is_same<Target, FirstType>::value) ?
			Index : IndexOfImpl<Index - 1, Target, TypeList<RestOfTypes...>>::ID);
	};

	template <typename Target, typename FirstType, typename... RestOfTypes>
	struct IndexOfImpl<0, Target, TypeList<FirstType, RestOfTypes...>>
	{
		static constexpr uint16 ID = (std::is_same<Target, FirstType>::value) ? 0 : -1;
	};

	template <typename Target, typename TypeListT>
	constexpr uint64 IndexOf =
		((IndexOfImpl<TypeListSize<TypeListT> - 1, Target, TypeListT>::ID > uint64{63}) ?
		(65) :
		(TypeListSize<TypeListT> - IndexOfImpl<TypeListSize<TypeListT> - 1, Target, TypeListT>::ID - 1));

	// Works similar to the previous example with TypeAt

	// ############################################################################################

///////////////////////////////////////////////////////////////////////////////////////////////////

	namespace metautil
	{
		template <typename TypeListT>
		struct PrintImpl
		{
			template <size_t Index>
			static void Print()
			{
				using Type = TypeAt<Index, TypeListT>;
				std::cout << ecs::util::type_name_to_string<Type>() << std::endl;
			}

			template <size_t... Indices>
			static void PrintAll(std::index_sequence<Indices...>)
			{
				(Print<Indices>(), ...);
			}

			void operator()() const
			{
				PrintAll(std::make_index_sequence<TypeListSize<TypeListT>>{});
			}
		};

		template <typename TypeListT>
		constexpr PrintImpl<TypeListT> Print{};

	// ############################################################################################

		template <typename TypeListT>
		struct TupleOfTypesImpl;

		template <typename... Typepack>
		struct TupleOfTypesImpl<TypeList<Typepack...>>
		{
			using Tuple = std::tuple<Typepack...>;
		};

		template <typename TypeListT>
		using TupleOfTypes = typename TupleOfTypesImpl<TypeListT>::Tuple;

		template <typename TypeListT>
		struct TupleOfVectorsOfTypesImpl;

		template <typename... Typepack>
		struct TupleOfVectorsOfTypesImpl<TypeList<Typepack...>>
		{
			using Tuple = typename std::tuple<std::vector<Typepack> ...>;
		};

		template <typename TypeListT>
		using TupleOfVectorsOfTypes = typename TupleOfVectorsOfTypesImpl<TypeListT>::Tuple;
	}  // namespace metautil
}  // namespace meta
}  // namespace ecs
