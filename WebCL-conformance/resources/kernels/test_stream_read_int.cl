__kernel void test_stream_read_int(
    __global int *dst)
{
    dst[0] = ((1<<16)+1);
}
