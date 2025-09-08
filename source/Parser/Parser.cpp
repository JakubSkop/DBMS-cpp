#include "Parser.hpp"

using namespace DB;

Expr DB::AND(Expr lhs, Expr rhs){
    Expr e;
    e.value = BinaryOperator(std::make_unique<Expr>(std::move(lhs)), BinaryLogicalOperator::AND ,std::make_unique<Expr>(std::move(rhs)));
    return e;
};

Expr DB::OR(Expr lhs, Expr rhs){
    Expr e;
    e.value = BinaryOperator(std::make_unique<Expr>(std::move(lhs)), BinaryLogicalOperator::OR ,std::make_unique<Expr>(std::move(rhs)));
    return e;
};

Expr DB::EQUALS(FieldOrValue lhs, FieldOrValue rhs){
    Expr e;
    e.value = Condition(std::move(lhs), ComparisonOperator::EQ, std::move(rhs));
    return e;
}