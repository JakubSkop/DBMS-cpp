#include "Table.hpp"
#include "Parser/Parser.hpp"

#include <iostream>
#include <string>


using namespace DB;
using enum TypeName;
using enum ComparisonOperator;

int main(){

    //auto results = select("Customers", {"Students"}, [](){} );

    //Customers.addColumn<BOOL>("Registered");

    //Customers.addColumn("Name", STRING);
    //Customers.addColumn("Age", INT);
    auto st = FixedString<64>("Hello");

    auto expr = AND(EQUALS(field("FirstName"), field("Surname")), EQUALS(field("AGE"), value(5)));

    auto p = Page(1);
    p.getData() = p.getData();

    //TypeName x = BOOL;
    //Customers.addColumn("Registered", x);

    return 0;
}
