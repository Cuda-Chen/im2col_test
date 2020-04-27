#include <iostream>
#include <iomanip>

using namespace std;

void im2col(int *data_im, int channels, int height, int width,
            int ksize, int stride, int pad, int *data_col);
void col2im(int *data_col, int channels, int height, int width,
            int ksize, int stride, int pad, int *data_im);
int im2col_get_pixel(int *im, int height, int width, int channels,
                     int row, int col, int channel, int pad);
void col2im_add_pixel(int *im, int height, int width, int channels,
                      int row, int col, int channel, int pad, int val);

void testConv1Channel();

int main()
{
    testConv1Channel();
        
    return 0;
}

void testConv1Channel()
{
    // A 1x5x5 matrix (CHW)
    int in_c = 1;
    int in_h = 5;
    int in_w = 5;
    /*int input[] = {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    };*/
    int in[] = {
        1, 2, 3, 4, 5,
        6, 7, 8, 9, 10,
        11, 12, 13, 14, 15,
        16, 17, 18, 19, 20,
        21, 22, 23, 24, 25
    };

    // An identity kernel
    // with 1x1x3x3 dimension (NCHW)
    int kernel_num = 1;
    int kernel_c = 1;
    int kernel_h = 3;
    int kernel_w = 3;
    int kernel[] = {
        0, 0, 0,
        0, 1, 0,
        0, 0, 0
    };

    // Padding and stride
    int pad = 1;
    int stride = 1;

    // A output matrix (CHW)
    // the dimension should be 1x3x3
    int out_c = kernel_num;
    int out_h = (in_h + 2 * pad - kernel_h) / stride + 1;
    int out_w = (in_w + 2 * pad - kernel_w) / stride + 1;
    int *out = new int[out_c * out_h * out_w];

    // temp array to store column
    int data_col_size = (in_c * kernel_h * kernel_w) * (out_h * out_w);
    int *data_col = new int[data_col_size];

    // Starts to do convolution using im2col
    im2col(in, in_c, in_h, in_w,
           kernel_h, stride, pad, data_col);    

    // Output the result
    int line_counter = 0;
    for(int i = 0; i < data_col_size; i++)
    {
        cout << setw(4) << data_col[i] << " ";
        line_counter++;
        if(line_counter % (out_h * out_w) == 0)
        {
            cout << endl;
        }
    }
}

void im2col(int *data_im, int channels, int height, int width,
            int ksize, int stride, int pad, int *data_col)
{
    int height_col = (height + 2 * pad - ksize) / stride + 1;
    int width_col = (width + 2 * pad - ksize) / stride + 1;
    int channels_col = channels * ksize * ksize;

    for(int c = 0; c < channels_col; c++)
    {
        int w_offset = c % ksize;
        int h_offset = (c / ksize) % ksize;
        int c_im = c / ksize / ksize;
        for(int h = 0; h < height_col; h++)
        {
            for(int w = 0; w < width_col; w++)
            {
                int im_row = h_offset + h * stride;
                int im_col = w_offset + w * stride;
                int col_index = (c * height_col + h) * width_col + w;
                data_col[col_index] = im2col_get_pixel(data_im, height, width, channels,
                                      im_row, im_col, c_im, pad);
            }
        }
    }
}

void col2im(int *data_col, int channels, int height, int width,
            int ksize, int stride, int pad, int *data_im)
{
    int height_col = (height + 2 * pad - ksize) / stride + 1;
    int width_col = (width + 2 * pad - ksize) / stride + 1;
    int channels_col = channels * ksize * ksize;

    for(int c = 0; c < channels_col; c++)
    {
        int w_offset = c % ksize;
        int h_offset = (c / ksize) % ksize;
        int c_im = c / ksize / ksize;
        for(int h = 0; h < height_col; h++)
        {
            for(int w = 0; w < width_col; w++)
            {
                int im_row = h_offset + h * stride;
                int im_col = w_offset + w * stride;
                int col_index = (c * height_col + h) * width_col + w;
                int val = data_col[col_index];
                col2im_add_pixel(data_im, height, width, channels,
                                 im_row, im_col, c_im, pad, val);
            }
        }
    }
}

int im2col_get_pixel(int *im, int height, int width, int channels,
                     int row, int col, int channel, int pad)
{
    row -= pad;
    col -= pad;

    if(row < 0 || col < 0 || row >= height || col >= width)
    {
        return 0;
    }

    return im[(channel * height + row) * width + col];
}

void col2im_add_pixel(int *im, int height, int width, int channels,
                      int row, int col, int channel, int pad, int val)
{
    row -= pad;
    col -= pad;

    if(row < 0 || col < 0 || row >= height || col >= width)
    {
        return;
    }

    im[(channel * height + row) * width + col] += val;
}
