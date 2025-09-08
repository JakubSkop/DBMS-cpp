#pragma once

#include "Typelist.hpp"
#include "CustomTypes/fixedString.hpp"
#include "Database_Constants.hpp"
#include "Database_Types.hpp"
#include "Table.hpp"

#include <string>
#include <functional>
#include <variant>
#include <array>
#include <memory>


namespace DB{

    class Interface{

        Table retrieve(const std::string& tableName);

        public:
            Table create(const std::string& tableName, std::vector<ColumnTypePair> columnTypePairs);

            void insert(const std::string& tableName, Row row);

            void update(const std::string& tableName, std::function<bool()> predicate, std::vector<UpdatePair> updatePairs);

            Table select(const std::string& tableName, std::function<bool()> predicate);

            void drop(const std::string& tableName);

            void renameColumn(const std::string& tableName, const std::string& oldColumnName, const std::string& newColumnName);
    };

};