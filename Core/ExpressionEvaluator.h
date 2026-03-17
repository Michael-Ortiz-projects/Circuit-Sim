#pragma once
#include <string>

class ExpressionEvaluator
{
public:
    ExpressionEvaluator() {}
    ExpressionEvaluator(const std::string& expression);

    void setExpression(const std::string& expression);

    double evaluate(double x) const;

private:

    struct Implementation;
    Implementation* implementation;
};