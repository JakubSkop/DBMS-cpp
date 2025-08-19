#pragma once

#include "Typelist.hpp"

#include <string>
#include <functional>
#include <map>
#include <variant>
#include <array>
#include <memory>


namespace DB{

    using Types = Typelist<bool, int, double, std::string>;
    enum TypeName{   BOOL, INT, DOUBLE,      STRING};

    using Variant = variantOfTypes<Types>;
    using VariantVector = variantofVectors<Types>;

    /*
    template <TypeName Enum> 
    struct EnumToTypeHelper{
        using type = std::tuple_element_t<static_cast<int>(Enum), tupleOfTypes<Types>>;
    };
    template <TypeName Enum>
    using EnumToTypeMapping = EnumToTypeHelper<Enum>::type;
    */

    template <template<typename> typename Wrapper>
    class MappingStruct{

        template<class... Ts> 
        static inline std::array<variantofWrappedTypes<Wrapper, Typelist<Ts...>>, Typelist<Ts...>::count> typelist_to_array_of_wrappers(Typelist<Ts...>){
            return std::array<variantofWrappedTypes<Wrapper, Typelist<Ts...>>, Typelist<Ts...>::count>{Wrapper<Ts>()...};
        }

        public:
            static inline const std::array<variantofWrappedTypes<Wrapper, Types>, Types::count> MappingArray{typelist_to_array_of_wrappers(Types())};
    };




    using UpdatePair = std::pair<std::string, Variant>;
    using ColumnTypePair = std::pair<TypeName, std::string>;



    class Row{
        std::vector<Variant> fields;
    };

    struct Column{
        std::string name;
        TypeName type;
        VariantVector data;
    };

    class Table{
        std::vector<Column> columns;
        
        public:
            void addRow(std::vector<Types> row);

            void addColumn(const std::string& name, TypeName type);
    };

    enum class ComparisonOperator{EQ, L, G, LEQ, GEQ, NEQ};
    using FieldOrValue = std::variant<Variant, std::string>;

    struct Expr;
    using ExprPtr = std::unique_ptr<Expr>;

    class Predicate{
        FieldOrValue operand1;
        FieldOrValue operand2;
        ComparisonOperator opcode;
    };

    enum class BLogicalOperator{AND, OR};
    enum class ULogicalOperator{NOT};

    struct BinaryOperator{
        ExprPtr Left;
        BLogicalOperator opcode;
        ExprPtr Right;
    };
    struct UnaryOperator{
        ULogicalOperator opcode;
        ExprPtr Right;
    };

    struct Expr{
        std::variant<Predicate, BinaryOperator, UnaryOperator> value;
    };




    class Interface{

        Table retrieve(const std::string& tableName);

        public:
            Table create(const std::string& tableName, std::vector<ColumnTypePair> columnTypePairs);

            void insert(const std::string& tableName, Row row);

            void update(const std::string& tableName, std::function<bool()> predicate, std::vector<UpdatePair> updatePairs);

            Table select(const std::string& tableName, std::function<bool()> predicate);

            void drop(const std::string& tableName);

            void destroy(const std::string& tableName, std::function<bool()> predicate);

            void addColumn(const std::string& tableName, ColumnTypePair columnTypePair);

            void dropColumn(const std::string& tableName, const std::string& columnName);

            void renameColumn(const std::string& tableName, const std::string& oldColumnName, const std::string& newColumnName);
    };

};