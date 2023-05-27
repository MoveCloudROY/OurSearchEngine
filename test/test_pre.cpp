#include "IndexBuilder.h"
#include "LibraryBuilder.h"

int main() {
    SG::LibraryBuilder lb{"../assets/material"};
    lb.build();


    SG::IndexBuilder obj{};
    obj.build();
    obj.dumpFst("../assets/library/fst.lib");
    obj.dumpSkipList("../assets/library/skl");
}