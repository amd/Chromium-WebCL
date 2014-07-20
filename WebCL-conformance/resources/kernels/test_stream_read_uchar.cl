__kernel void test_stream_read_uchar(
    __global uchar *dst)
{
    dst[0] = (uchar)'w';
}
