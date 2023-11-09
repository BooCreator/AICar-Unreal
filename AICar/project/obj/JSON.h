#include "Vec.h"
#include "Logger.h"

struct JSON {
    public:
        Vec force; // local
        double gas;
        double rotate;
        Logger log;

		JSON(Vec force, double gas, double rotate, Logger log) { this->force = force; this->gas = gas; this->rotate = rotate; this->log = log; }

};