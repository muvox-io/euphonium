#include <iostream>
#include "Core.h"
#include "HTTPServer.h"

int main(int argc, char *argv[])
{
    auto core = std::make_shared<Core>();
    core->registerChildren();
    core->logAvailableServices();
    return 0;
}