__kernel void test_stream_read_long(
    __global long *dst)
{
    dst[0] = ((1L<<32)+1L);
}
