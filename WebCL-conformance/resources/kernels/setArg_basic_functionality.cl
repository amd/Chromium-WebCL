__kernel void kernelChar(
   __global char* input,
   __global char* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}
__kernel void kernelUChar(
   __global uchar* input,
   __global uchar* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}

__kernel void kernelShort(
   __global short* input,
   __global short* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}
__kernel void kernelUShort(
   __global ushort* input,
   __global ushort* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}

__kernel void kernelInt(
   __global int* input,
   __global int* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}
__kernel void kernelUInt(
   __global uint* input,
   __global uint* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}

__kernel void kernelLong(
   __global long* input,
   __global long* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}
__kernel void kernelULong(
   __global ulong* input,
   __global ulong* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}

__kernel void kernelFloat(
   __global float* input,
   __global float* output,
   unsigned int count
   )
{
    unsigned int i = get_global_id(0);
    if (i < count)
        output[i] = input[i];
}
