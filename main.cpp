#include "core.h"
#include "uci.h"

int main()
{
    core engine = core();

    uciLoop(engine);

    return 0;
}