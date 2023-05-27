#include "Queryer.h"
using namespace SG;
int main()
{
    Queryer& queryer=Queryer::getInstance();
    queryer.predict_sentence("使用寿命","寿命");

}