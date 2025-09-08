#pragma once 

#include "Database_Types.hpp"

#include <string>
#include <memory>


namespace DB {
    enum class ComparisonOperator{EQ, L, G, LEQ, GEQ, NEQ};
    using FieldOrValue = std::variant<Variant, std::string>;

    struct Expr;
    using ExprPtr = std::unique_ptr<Expr>;

    struct Condition{
        FieldOrValue operand1;
        ComparisonOperator opcode;
        FieldOrValue operand2;
    };

    enum class BinaryLogicalOperator{AND, OR};

    struct BinaryOperator{
        ExprPtr Left;
        BinaryLogicalOperator opcode;
        ExprPtr Right;
    };

    struct Expr{
        std::variant<Condition, BinaryOperator> value;
    };

    Expr AND(Expr lhs, Expr rhs);
    Expr OR(Expr lhs, Expr rhs);
    Expr EQUALS(FieldOrValue lhs, FieldOrValue rhs);

    inline FieldOrValue field(const char* name) {
        return FieldOrValue(std::string(name));
    }

    inline FieldOrValue value(Variant var) {
        return FieldOrValue(var);
    }
}