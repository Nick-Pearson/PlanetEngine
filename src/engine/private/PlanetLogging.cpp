#include "PlanetLogging.h"

void PlanetLogging::init_logging()
{
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%l] [%t] [%s] %v");
}