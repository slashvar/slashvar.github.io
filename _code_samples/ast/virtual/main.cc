#include <iostream>
#include <memory>
#include <unordered_map>

#include "ast.hh"

using namespace inheritance::ast;

struct Pretty_printer : public Visitor
{
    void visit(Integer* i) override
    {
        std::cout << i->value;
    }

    void visit(Variable* var) override
    {
        std::cout << var->name;
    }

    void visit(Bin_op* bop) override
    {
        std::cout << "(";
        bop->lhs->accept(this);
        std::cout << " " << bop->op << " ";
        bop->rhs->accept(this);
        std::cout << ")";
    }

    void visit(Let* let) override
    {
        std::cout << "let " << let->var_name << " = ";
        let->var_expr->accept(this);
        std::cout << " in ";
        let->in_expr->accept(this);
    }

    void visit(Node* node) override
    {
        node->accept(this);
    }
};

struct Undefined_variable : public std::exception
{
    Undefined_variable(std::string v) : msg(std::string("unknown variable ") + v) {}

    const char* what() const noexcept override
    {
        return msg.c_str();
    }

    std::string msg;
};

struct Undefined_operator : public std::exception
{
    Undefined_operator(std::string v) : msg(std::string("unknown operator ") + v) {}

    const char* what() const noexcept override
    {
        return msg.c_str();
    }

    std::string msg;
};

struct Eval : public Visitor
{
    void visit(Node* node) override
    {
        node->accept(this);
    }

    void visit(Integer* i) override
    {
        res = i->value;
    }

    void visit(Variable* var) override
    {
        if (not env.contains(var->name)) {
            throw Undefined_variable(var->name);
        }
        res = env[var->name];
    }

    void visit(Bin_op* bop) override
    {
        bop->lhs->accept(this);
        int lres = res;
        bop->rhs->accept(this);

        if (bop->op == "+") {
            res = lres + res;
            return;
        }
        if (bop->op == "-") {
            res = lres - res;
            return;
        }
        if (bop->op == "*") {
            res = lres * res;
            return;
        }
        if (bop->op == "/") {
            res = lres / res;
            return;
        }
        throw Undefined_operator(bop->op);
    }

    void visit(Let* let) override
    {
        let->var_expr->accept(this);
        env[let->var_name] = res;

        Eval sub(*this);
        let->in_expr->accept(&sub);
        res = sub.res;
    }

    int res = 0;

    std::unordered_map<std::string, int> env;
};

int main()
{
    auto expr =
        let("x", integer(3), bin_op(bin_op(integer(1), integer(2), "+"), variable("x"), "+"));
    auto pretty_printer = std::make_unique<Pretty_printer>();
    pretty_printer->visit(expr.get());
    std::cout << "\n";

    Eval eval {};
    eval.visit(expr.get());
    std::cout << eval.res << std::endl;
}
