#include "Application.hpp"

int main()
{
    Application::init("Rubik Cube", { 800, 800 });
    Application::run();

    return 0;
}
