#pragma once 

#include "Database_Constants.hpp"
#include "Database_Types.hpp"

#include <iostream>
#include <fstream>
#include <list>
#include <optional>
#include <map>
#include <memory>

namespace DB {

    class Row{
        std::vector<Variant> fields;
    };

    struct Column{
        std::string name;
        TypeName type;
        VariantVector data;
    };

    struct Header{
        unsigned char NumberOfColumns;
        std::array<TypeInt, 255> ColumnTypes;
        std::array<NameType, 255> ColumnNames;
        NameType TableName;
        ID_Int FirstPageID;
        ID_Int LastPageID;
        ID_Int RootID;
    };

    using PageRowPair = std::pair<ID_Int, RowInt>;
    using KeyArray = std::array<int, BP_TREE_SIZE>; 
    using KeyRange = std::pair<int, int>;
    using OptionalKeyPagePair = std::optional<std::pair<int, ID_Int>>;

    struct DataPage_Inner{
        ID_Int NextPageID; //Next Data Page
        RowInt NumberOfRows;
        std::vector<Column> columns;
    };

    struct BPPage_Inner{
        ID_Int ParentPageID;
        size_t KeyCount;
        DB::KeyArray Keys;
    };

    struct NodePage_Inner : public BPPage_Inner{
        std::array<ID_Int, BP_TREE_SIZE + 1> ChildPageIds;
    };

    struct LeafPage_Inner : public BPPage_Inner{
        std::array<PageRowPair, BP_TREE_SIZE> ChildPages;
    };

    class Page{
        using Inner = std::variant<DataPage_Inner, NodePage_Inner, LeafPage_Inner>;

        ID_Int pageId;
        Inner data;
        bool dirty = false;

        public:
            Page(ID_Int Id);
            ID_Int getId() const;
            const Inner& getData() const;
            Inner& getData();
    };


    class Serializer{

        std::shared_ptr<Header> currentHeaderPtr;

        std::fstream file;

        public:
            Serializer(const std::string& filename);
            Header getHeader();
            void writeHeader(Header header);
            Page getPage(ID_Int PageID);
            void writePage(Page page);
            ID_Int createPage(); //makes a dataPage
            ID_Int createPage(NodePage_Inner data);
            ID_Int createPage(LeafPage_Inner data);

    };

    class Buffer{
        Serializer& serializer;
        std::list<Page> pageList; //stores all the hot pages in RAM, hottest pages being at the front

        using ListIter = decltype(pageList)::const_iterator;
        std::map<ID_Int, ListIter> bufferMap;

        public:
            Buffer(Serializer& serializer);
            const Page& getPage(ID_Int pageId);
            void writePage(Page& page);
            ID_Int createDataPage(DataPage_Inner data);
            ID_Int createNodePage(NodePage_Inner data);
            ID_Int createLeafPage(LeafPage_Inner data);
    };

    class Table{
        Buffer buffer;
        Serializer serializer;
        std::shared_ptr<Header> currentHeaderPtr;
        
        public:
            void addRow(std::vector<Types> row);
    };

}