#include "gator_path_app.h"

int main(int argc, char* argv[]) {
    GatorPathApp app;
    app.run(argc > 1 ? argv[1] : "");
    return 0;
}
