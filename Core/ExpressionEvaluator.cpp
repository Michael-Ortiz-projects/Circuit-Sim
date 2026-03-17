#include "ExpressionEvaluator.h"
#include <iostream>
#include "exprtk.hpp"

struct ExpressionEvaluator::Implementation
{
    double t;

    exprtk::symbol_table<double> table;
    exprtk::expression<double> expression;
    exprtk::parser<double> parser;

    Implementation(const std::string& expr)
    {
        table.add_variable("t", t);
        table.add_constants();

        expression.register_symbol_table(table);

        std::cout << "parser compile result: " << parser.compile(expr, expression) << "\n";
    }

    double eval(double value)
    {
        t = value;
        return expression.value();
    }
};

ExpressionEvaluator::ExpressionEvaluator(const std::string& expr)
{
    implementation = new Implementation(expr);
}

double ExpressionEvaluator::evaluate(double x) const
{
    return implementation->eval(x);
}

void ExpressionEvaluator::setExpression(const std::string& expr) {
    implementation = new Implementation(expr);
}