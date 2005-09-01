#include <lv_function.hpp>

#ifdef LVCPP_FUNCTION_TEST

#include <iostream>

int print_hello_world ()
{
    std::cout << "Free function" << std::endl;
    return 0;
}

struct Action
{
    int execute ()
    {
        std::cout << "Member function" << std::endl;
        return 1;
    }

    int operator () ()
    {
        std::cout << "Function" << std::endl;
        return 2;
    }
};

int main ()
{
    using Lv::Function;

    {
        Function<int> functor (&print_hello_world);
        std::cout << "Result: " << functor () << std::endl;
    }

    {
        Action action;

        Function<int> functor(&action, &Action::execute);
        std::cout << "Result: " << functor () << std::endl;
    }

    {
        Action action;
        Function<int> functor(action);
        std::cout << "Result: " << functor () << std::endl;
    }
    
    {
        Function<int> a (&print_hello_world);
        Function<int> b;

        b = a;
        a ();
    }

    {
        Function<int> a (&print_hello_world);
        Function<int> b;

        b = a;
        a ();
    }
}

#endif // #ifdef LVCPP_FUNCTION_TEST
