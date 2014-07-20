__kernel void test_stream_read_uint(
    __global uint *dst)
{
    dst[0] = ((1U<<16)+1U);
}
