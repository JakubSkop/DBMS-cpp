#include "ProgrammingInterface.hpp"

using namespace DB;

void Table::addColumn(const std::string& name, TypeName type){
    Column col{name, type, {}};

    col.data = MappingStruct<std::vector>::MappingArray[type];

    columns.push_back(std::move(col));

};