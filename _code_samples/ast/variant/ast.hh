#pragma once

#include <memory>
#include <variant>

namespace variant::ast {

struct Integer;
struct Variable;
struct Bin_op;
struct Let;

using Node = std::variant<Integer, Variable, Bin_op, Let>;

struct Integer
{
    Integer(int v) : value(v) {}
    int value;
};

std::unique_ptr<Node> integer(int value)
{
    return std::make_unique<Node>(Integer { value });
}

struct Variable
{
    Variable(std::string v) : name(std::move(v)) {}
    std::string name;
};

std::unique_ptr<Node> variable(std::string var)
{
    return std::make_unique<Node>(Variable(std::move(var)));
}

struct Bin_op
{
    Bin_op(std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& r, std::string o) :
      lhs(std::move(l)), rhs(std::move(r)), op(std::move(o))
    {}

    std::unique_ptr<Node> lhs;
    std::unique_ptr<Node> rhs;

    std::string op;
};

std::unique_ptr<Node> bin_op(std::unique_ptr<Node>&& lhs,
                             std::unique_ptr<Node>&& rhs,
                             std::string             op)
{
    return std::make_unique<Node>(Bin_op(std::move(lhs), std::move(rhs), std::move(op)));
}

struct Let
{
    Let(std::string v, std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& i) :
      var_name(std::move(v)), var_expr(std::move(l)), in_expr(std::move(i))
    {}

    std::string           var_name;
    std::unique_ptr<Node> var_expr;
    std::unique_ptr<Node> in_expr;
};

std::unique_ptr<Node> let(std::string v, std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& i)
{
    return std::make_unique<Node>(Let(std::move(v), std::move(l), std::move(i)));
}

};
