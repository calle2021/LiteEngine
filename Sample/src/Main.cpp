#include "PIXL.h"

class Game : public PIXL::GameApp {
public:
    Game() 
    {
    }
};
#ifdef _WIN32
CREATE_APPLICATION(Game);
#else
#error "Only support Windows"
#endif
