#pragma once

#include "Typelist.hpp"
#include "fixedString.hpp"

#include <string>
#include <functional>
#include <map>
#include <variant>
#include <array>
#include <memory>
#include <iostream>
#include <fstream>
#include <list>
#include <optional>


namespace DB{

    constexpr size_t PAGE_SIZE = 4096;
    constexpr size_t BUFFER_SIZE = 256; //The max number of pages stored at once in the buffer
    constexpr size_t BP_TREE_SIZE = 64;

    using ID_Int = uint32_t;
    using TypeInt = uint8_t;
    using RowInt = uint16_t; //Used to store the number of rows
    using NameType = FixedString<64>;


    using Types = Typelist<bool, int, double, FixedString<64>>;
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

    struct Header{
        char NumberOfColumns;
        std::array<TypeInt, 255> ColumnTypes;
        std::array<NameType, 255> ColumnNames;
        NameType TableName;
        ID_Int FirstPageID;
        ID_Int LastPageID;
        ID_Int RootID;
    };

    struct DataPage{
        ID_Int PageID;
        ID_Int NextPageID;
        RowInt NumberOfRows;
        TypeInt PageType;
        std::vector<Column> columns;
    };



    using PageRowPair = std::pair<ID_Int, RowInt>;
    using PageRowPairs = std::vector<PageRowPair>;
    using KeyArray = std::array<Variant, BP_TREE_SIZE>;
    using KeyRange = std::pair<Variant&, Variant&>;
    using OptionalKeyPagePair = std::optional<std::pair<Variant, ID_Int>>;

    struct BPPage{
        ID_Int PageID;
        ID_Int ParentPageID;

        size_t KeyCount;
        DB::KeyArray Keys;
    };

    struct NodePage : BPPage{
        std::array<ID_Int, BP_TREE_SIZE + 1> ChildPageIds;
    };

    struct LeafPage : BPPage{
        std::array<PageRowPair, BP_TREE_SIZE> ChildPages;
    };



    using Page = std::variant<DataPage, NodePage, LeafPage>;

    class Serializer{
        std::fstream file;

        public:
            Serializer(const std::string& filename);
            Header getHeader();
            void writeHeader(Header header);
            Page getPage(ID_Int PageID);
            void writePage(Page page);

    };

    class Buffer{

        Serializer& serializer;
        std::list<Page> pageList;

        using ListIter = decltype(pageList)::const_iterator;
        std::map<ID_Int, ListIter> bufferMap;


        public:
            Buffer(Serializer& serializer);
            Page getPage(ID_Int pageId);

    };

    class Table{
        Buffer buffer;
        Serializer serializer;
        Header header;
        
        public:
            void addRow(std::vector<Types> row);
    };



    enum class ComparisonOperator{EQ, L, G, LEQ, GEQ, NEQ};
    using FieldOrValue = std::variant<Variant, std::string>;

    struct Expr;
    using ExprPtr = std::unique_ptr<Expr>;

    class Condition{
        FieldOrValue operand1;
        FieldOrValue operand2;
        ComparisonOperator opcode;
    };

    enum class BLogicalOperator{AND, OR};

    struct BinaryOperator{
        ExprPtr Left;
        BLogicalOperator opcode;
        ExprPtr Right;
    };

    struct Expr{
        std::variant<Condition, BinaryOperator> value;
    };




    class Interface{

        Table retrieve(const std::string& tableName);

        public:
            Table create(const std::string& tableName, std::vector<ColumnTypePair> columnTypePairs);

            void insert(const std::string& tableName, Row row);

            void update(const std::string& tableName, std::function<bool()> predicate, std::vector<UpdatePair> updatePairs);

            Table select(const std::string& tableName, std::function<bool()> predicate);

            void drop(const std::string& tableName);

            //void addColumn(const std::string& tableName, ColumnTypePair columnTypePair);

            //void dropColumn(const std::string& tableName, const std::string& columnName);

            void renameColumn(const std::string& tableName, const std::string& oldColumnName, const std::string& newColumnName);
    };

};
