#include <iostream>
#include <bitset>
#include <sstream>

/*int convertToHex(int x)
{
    return std::bitset<16>{x};
}
*/
int main(int argc, char const *argv[])
{
    int x = 3614090360;
    int y;
    //std::cout << std::bitset<8>{x} << std::endl;
    std::istringstream("3614090360") >> std::hex >> y;
    std::cout << y << std::endl;

}
