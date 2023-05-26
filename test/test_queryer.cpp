#include "Queryer.h"
using namespace SG;
int main()
{
    Queryer& queryer=Queryer::getInstance();
    queryer.get("使用寿命",1,10);

}