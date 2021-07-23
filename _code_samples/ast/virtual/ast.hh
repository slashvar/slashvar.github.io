#pragma once

#include <memory>

namespace inheritance::ast {

struct Node;
struct Integer;
struct Variable;
struct Bin_op;
struct Let;

struct Visitor
{
    virtual void visit(Node*)     = 0;
    virtual void visit(Integer*)  = 0;
    virtual void visit(Variable*) = 0;
    virtual void visit(Bin_op*)   = 0;
    virtual void visit(Let*)      = 0;

    virtual ~Visitor() = default;
};

struct Node
{
    virtual void accept(Visitor*) = 0;

    virtual ~Node() = default;
};

struct Integer : public Node
{
    Integer(int v) : value(v) {}
    int value;

    void accept(Visitor* visitor) override
    {
        visitor->visit(this);
    }
};

std::unique_ptr<Node> integer(int value)
{
    return std::make_unique<Integer>(value);
}

struct Variable : public Node
{
    Variable(std::string v) : name(std::move(v)) {}
    std::string name;

    void accept(Visitor* visitor) override
    {
        visitor->visit(this);
    }
};

std::unique_ptr<Node> variable(std::string var)
{
    return std::make_unique<Variable>(std::move(var));
}

struct Bin_op : public Node
{
    Bin_op(std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& r, std::string o) :
      lhs(std::move(l)), rhs(std::move(r)), op(std::move(o))
    {}

    void accept(Visitor* visitor) override
    {
        visitor->visit(this);
    }

    std::unique_ptr<Node> lhs;
    std::unique_ptr<Node> rhs;

    std::string op;
};

std::unique_ptr<Node> bin_op(std::unique_ptr<Node>&& lhs,
                             std::unique_ptr<Node>&& rhs,
                             std::string             op)
{
    return std::make_unique<Bin_op>(std::move(lhs), std::move(rhs), std::move(op));
}

struct Let : public Node
{
    Let(std::string v, std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& i) :
      var_name(std::move(v)), var_expr(std::move(l)), in_expr(std::move(i))
    {}

    void accept(Visitor* visitor) override
    {
        visitor->visit(this);
    }
    std::string           var_name;
    std::unique_ptr<Node> var_expr;
    std::unique_ptr<Node> in_expr;
};

std::unique_ptr<Node> let(std::string v, std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& i)
{
    return std::make_unique<Let>(std::move(v), std::move(l), std::move(i));
}

};
