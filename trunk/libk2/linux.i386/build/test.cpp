#include <memory>
#include <cstdlib>
#include <stdio.h>
#include <string>

int main ()
{
    std::printf("1\n");
    //std::string msg("what's up");
    std::printf("2\n");
    int* pint = new int(3);
    std::printf("%d\n", *pint);
    std::auto_ptr<int>  ap(new int(4));
    std::printf("%d\n", *ap);
    //FILE*   pf = _fopen_r(0, 0, 0);
}
