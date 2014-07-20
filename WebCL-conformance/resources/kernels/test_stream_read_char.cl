__kernel void test_stream_read_char(
    __global char *dst)
{
    dst[0] = (char)'w';
}
