#pragma once 

#include "CustomTypes/fixedString.hpp"
#include "Typelist.hpp"
#include "CustomTypes/Bool.hpp"

#include <type_traits>


namespace DB{

    using ID_Int = uint32_t; //Used for PageIDs
    using TypeInt = uint8_t; //Used for storing the type of ID
    using RowInt = uint16_t; //Used to store the number of rows
    using NameType = FixedString<64>;

    using Types = Typelist<Bool, int, double, FixedString<64>>;
    enum TypeName{   BOOL, INT, DOUBLE,      STRING};

    using Variant = variantOfTypes<Types>;
    using VariantVector = variantOfVectors<Types>;

    using UpdatePair = std::pair<std::string, Variant>;
    using ColumnTypePair = std::pair<TypeName, std::string>;



    template <template<typename> typename Wrapper = std::type_identity>
    class MappingStruct{

        template<class... Ts> 
        static inline std::array<variantOfWrappedTypes<Wrapper, Typelist<Ts...>>, Typelist<Ts...>::count> typelist_to_array_of_wrappers(Typelist<Ts...>){
            return std::array<variantOfWrappedTypes<Wrapper, Typelist<Ts...>>, Typelist<Ts...>::count>{Wrapper<Ts>()...};
        }

        public:
            static inline const std::array<variantOfWrappedTypes<Wrapper, Types>, Types::count> MappingArray{typelist_to_array_of_wrappers(Types())};
    };

    template <>
    class MappingStruct<std::type_identity>{

        template<class... Ts> 
        static inline std::array<variantOfTypes<Typelist<Ts...>>, Typelist<Ts...>::count> typelist_to_array_of_types(Typelist<Ts...>){
            return std::array<variantOfTypes<Typelist<Ts...>>, Typelist<Ts...>::count>{Ts()...};
        }

        public:
            static inline const std::array<variantOfTypes<Types>, Types::count> MappingArray{typelist_to_array_of_types(Types())};
    };


}