__kernel void test_stream_read_ushort(
    __global ushort *dst)
{
    dst[0] = (unsigned short)((1<<8)+1);
}
