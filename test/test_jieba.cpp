#include "DivideCreator.h"
#include <iostream>
#include <string>

int main() {
    SG::Divider      d;
    SG::DivideResult r = d.divide("艺术");
    std::cout << r.lastWord << std::endl;

    std::string a           = "你好吗";
    std::string la          = "吗";
    std::string na          = "我很好";
    int         l           = la.length();
    size_t      lastWordPos = a.rfind(la);
    std::string tmp         = a;
    tmp.replace(lastWordPos, l, na);
    std::cout << tmp << std::endl;
}