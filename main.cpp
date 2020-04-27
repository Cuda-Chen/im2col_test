#include <iostream>

using namespace std;

int main()
{
    // A 1x5x5 matrix (CHW)
    int in_c = 1;
    int in_h = 5;
    int in_w = 5;
    int input[] = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };

    // An identity kernel
    // with 1x1x3x3 dimension (NCHW)
    int kernel_num = 1;
    int kernel_c = 1;
    int kernel_h = 3;
    int kernel_w = 3;
    int kernel[] = {
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0}
    };

    // Padding and stride
    int pad = 0;
    int stride = 1;

    // A output matrix (CHW)
    // the dimension should be 1x3x3
    int out_c = 1;
    int out_h = 3;
    int out_w = 3;
}
