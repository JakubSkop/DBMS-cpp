#pragma once

#include <string>
#include <functional>
#include <map>
#include <variant>


using DBTypes = std::variant<bool, int, double, std::string>;
using UpdatePair = std::pair<std::string, DBTypes>;
using ColumnTypePair = std::pair<std::string, std::string>;



class Record{
    std::vector<DBTypes> fields;
};

class Table{
    std::vector<std::vector<DBTypes>> fields;

    public:
        void addRow(std::vector<DBTypes> row);

        template <typename T>
        void addColumn(std::vector<DBTypes> column);
};


class DBPredicate{
    
    public:
        DBPredicate(std::array);
};


class Interface{

    Table retrieve(const std::string& tableName);

    public:
        Table create(const std::string& tableName, std::vector<ColumnTypePair> columnTypePairs);

        void insert(const std::string& tableName, Record record);

        void update(const std::string& tableName, std::function<bool(WrapperT)> predicate, std::vector<UpdatePair> updatePairs);

        Table select(const std::string& tableName, std::function<bool(WrapperT)> predicate);

        void drop(const std::string& tableName);

        void destroy(const std::string& tableName, std::function<bool(WrapperT)> predicate);

        void addColumn(const std::string& tableName, ColumnTypePair columnTypePair);

        void dropColumn(const std::string& tableName, const std::string& columnName);

        void renameColumn(const std::string& tableName, const std::string& oldColumnName, const std::string& newColumnName);
};
