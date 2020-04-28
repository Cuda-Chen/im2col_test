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
void naiveGEMM(int *out, int *kernel, int *in,
               int kernel_row, int kernel_col, int in_row, int in_col);

void testConv1Channel();
void testConv3Channel();

int main()
{
    //testConv1Channel();
    testConv3Channel();
        
    return 0;
}

void testConv1Channel()
{
    // A 1x5x5 matrix (CHW)
    int in_c = 1;
    int in_h = 5;
    int in_w = 5;
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
    // the dimension should be 1x5x5
    int out_c = kernel_num;
    int out_h = (in_h + 2 * pad - kernel_h) / stride + 1;
    int out_w = (in_w + 2 * pad - kernel_w) / stride + 1;
    int *out = new int[out_c * out_h * out_w];

    // temp array to store column
    int data_col_height = in_c * kernel_h * kernel_w;
    int data_col_width = out_h * out_w;
    int data_col_size = data_col_height * data_col_width;
    int *data_col = new int[data_col_size];

    int out_col_height = data_col_height;
    int out_col_width = data_col_width;
    int *out_col = new int[out_col_height * out_col_width];

    // Starts to do convolution using im2col
    im2col(in, in_c, in_h, in_w,
           kernel_h, stride, pad, data_col);    

    // Output data_col
    int line_counter = 0;
    cout << "data_col content:" << endl;
    for(int i = 0; i < data_col_size; i++)
    {
        cout << setw(4) << data_col[i] << " ";
        line_counter++;
        if(line_counter % (out_h * out_w) == 0)
        {
            cout << endl;
        }
    }

    // Output kernel
    // Note the dimension is changed from NCHW to Nx(CxKxK)
    int kernel_col_height = kernel_num;
    int kernel_col_width = kernel_c * kernel_h * kernel_w;
    line_counter = 0;
    cout << endl << "kernel content:" << endl;
    for(int i = 0; i < kernel_col_height; i++)
    {
        for(int j = 0; j < kernel_col_width; j++)
        {
            cout << setw(4) << kernel[i * kernel_col_width + j] << " ";
            line_counter++;
            if(line_counter % kernel_col_width == 0)
            {
                cout << endl;
            }
        }
    }

    // Apply GEMM
    naiveGEMM(out, kernel, data_col,
        kernel_col_height, kernel_col_width, data_col_height, data_col_width);

    // Output out_col
    // The dimension of out_col should be N x C x out_h x out_w
#if 0
    line_counter = 0;
    cout << endl << "out_col content:" << endl;
    for(int i = 0; i < out_col_height; i++)
    {
        for(int j = 0; j < out_col_width; j++)
        {
            cout << setw(4) << out_col[i * out_col_width + j] << " ";
            line_counter++;
            if(line_counter % out_col_width == 0)
            {
                cout << endl;
            }
        }
    }
#endif

    // Then do col2im
    // Currently no need to do this

    // Output out
    // The dimension of out should be N x out_h x out_w
    line_counter = 0;
    cout << endl << "out content:" << endl;
    for(int i = 0; i < out_c * out_h * out_w; i++)
    {
        cout << setw(4) << out[i] << " ";
        line_counter++;
        if(line_counter % out_w == 0)
        {
            cout << endl;
        }
    }

    delete[] out;
    delete[] data_col;
    delete[] out_col;
}

void testConv3Channel()
{
    // A 3x5x5 input matrix
    int in_c = 3, in_h = 5, in_w = 5;
    int in[] = {
        /* channel 0 */
        2, 1, 1, 0, 1,
        1, 1, 0, 2, 1,
        2, 0, 0, 2, 2,
        2, 0, 1, 1, 1,
        1, 2, 2, 2, 0,
        /* channel 1 */
        0, 0, 0, 2, 1,
        0, 0, 2, 0, 2,
        0, 2, 2, 2, 1,
        0, 0, 2, 0, 1,
        0, 0, 2, 1, 1,
        /* channel 2 */
        0, 2, 2, 0, 2,
        0, 1, 1, 0, 2,
        1, 2, 2, 1, 0,
        2, 2, 0, 2, 2,
        2, 0, 2, 2, 0
    };

    // An 2x3x3x3 identity kernel
    int kernel_num = 2, kernel_c = 3, kernel_h = 3, kernel_w = 3;
    int kernel[] = {
        /* filter w0 */
        /* channel 0 */
        0, 0, 1,
        0, 0, -1,
        1, 0, 0,
        /* channel 1 */
        -1, 0, 1,
        1, 1, 0,
        0, 0, 0,
        /* channel 2 */
        1, -1, -1,
        1, 0, -1,
        1, -1, 0,

        /* filter w1 */
        /* channel 0 */
        1, 1, 0,
        0, 1, 1,
        1, 1, 0,
        /* channel 1 */
        -1, 1, -1,
        -1, 0, 1,
        -1, 0, 1,
        /* channel 2 */
        1, -1, 1,
        1, 1, 0,
        -1, -1, 1
    };

    // Padding and stride
    int pad = 1, stride = 2;

    // A 2x3x3 output matrix
    int out_c = 2;
    int out_h = (in_h + 2 * pad - kernel_h) / stride + 1;
    int out_w = (in_w + 2 * pad - kernel_w) / stride + 1;
    int *out = new int[out_c * out_h * out_w];

    // temp array to store input column
    int in_col_height = in_c * kernel_h * kernel_w;
    int in_col_width = out_h * out_w;
    int in_col_size = in_col_height * in_col_width;
    int *in_col = new int[in_col_size];

    // Starts to do convolution using im2col
    im2col(in, in_c, in_h, in_w,
           kernel_h, stride, pad, in_col);

    // Output in_col
    int line_counter = 0;
    cout << "in_col content:" << endl;
    for(int i = 0; i < in_col_size; i++)
    {
        cout << setw(2) << in_col[i] << " ";
        line_counter++;
        if(line_counter % (out_h * out_w) == 0)
        {
            cout << endl;
        }
    }

    // Output kernel
    // Note the dimension is changed from NCHW to Nx(CxKxK)
    int kernel_col_height = kernel_num;
    int kernel_col_width = kernel_c * kernel_h * kernel_w;
    line_counter = 0;
    cout << endl << "kernel content:" << endl;
    for(int i = 0; i < kernel_col_height; i++)
    {
        for(int j = 0; j < kernel_col_width; j++)
        {
            cout << setw(2) << kernel[i * kernel_col_width + j] << " ";
            line_counter++;
            if(line_counter % kernel_col_width == 0)
            {
                cout << endl;
            }
        }
    }

    // Apply GEMM
    naiveGEMM(out, kernel, in_col,
        kernel_col_height, kernel_col_width, in_col_height, in_col_width);

    // Output out
    // The dimension of out should be N x out_h x out_w
    line_counter = 0;
    cout << endl << "out content:" << endl;
    for(int i = 0; i < out_c * out_h * out_w; i++)
    {
        cout << setw(2) << out[i] << " ";
        line_counter++;
        if(line_counter % out_w == 0)
        {
            cout << endl;
        }
    }

    delete[] out;
    delete[] in_col;
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

void naiveGEMM(int *out, int *kernel, int *in,
               int kernel_row, int kernel_col, int in_row, int in_col)
{
    /* The output matrix dimension will be kernel_row * in_col */

    for(int i = 0; i < kernel_row; i++)
    {
        for(int j = 0; j < in_col; j++)
        {
            out[i * in_col + j] = 0;
            for(int k = 0; k < in_row; k++)
            {
                out[i * in_col + j] +=
                    kernel[i * kernel_col + k] *
                    in[k * in_col + j];
            }
        }
    }

}
