#include <iostream>
#include <memory>
#include <unordered_map>
#include <variant>

#include "ast.hh"

using namespace variant::ast;

struct Pretty_printer
{
    void operator()(const Integer& i)
    {
        std::cout << i.value;
    }

    void operator()(const Variable& var)
    {
        std::cout << var.name;
    }

    void operator()(const Bin_op& bop)
    {
        std::cout << "(";
        std::visit(*this, *bop.lhs);
        std::cout << " " << bop.op << " ";
        std::visit(*this, *bop.rhs);
        std::cout << ")";
    }

    void operator()(const Let& let)
    {
        std::cout << "let " << let.var_name << " = ";
        std::visit(*this, *let.var_expr);
        std::cout << " in ";
        std::visit(*this, *let.in_expr);
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

struct Eval
{
    void operator()(const Integer& i)
    {
        res = i.value;
    }

    void operator()(const Variable& var)
    {
        if (not env.contains(var.name)) {
            throw Undefined_variable(var.name);
        }
        res = env[var.name];
    }

    void operator()(const Bin_op& bop)
    {
        std::visit(*this, *bop.lhs);
        int lres = res;
        std::visit(*this, *bop.rhs);

        if (bop.op == "+") {
            res = lres + res;
            return;
        }
        if (bop.op == "-") {
            res = lres - res;
            return;
        }
        if (bop.op == "*") {
            res = lres * res;
            return;
        }
        if (bop.op == "/") {
            res = lres / res;
            return;
        }
        throw Undefined_operator(bop.op);
    }

    void operator()(const Let& let)
    {
        std::visit(*this, *let.var_expr);
        env[let.var_name] = res;

        Eval sub(*this);
        std::visit(sub, *let.in_expr);
        res = sub.res;
    }

    int res = 0;

    std::unordered_map<std::string, int> env;
};

int main()
{
    auto expr =
        let("x", integer(3), bin_op(bin_op(integer(1), integer(2), "+"), variable("x"), "+"));
    Pretty_printer pretty_printer {};
    std::visit(pretty_printer, *expr);
    std::cout << "\n";

    Eval eval {};
    std::visit(eval, *expr);
    std::cout << eval.res << std::endl;
}
