__kernel void test_stream_read_float(
    __global float *dst)
{
    dst[0] = (float)3.40282346638528860e+38;
}
