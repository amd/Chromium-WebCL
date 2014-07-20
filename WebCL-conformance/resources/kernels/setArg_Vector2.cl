__kernel void kernelChar(
   char2 input,
   __global char2* output)
{
   output[0] = input;
}
__kernel void kernelUChar(
   uchar2 input,
   __global uchar2* output)
{
   output[0] = input;
}

__kernel void kernelShort(
   short2 input,
   __global short2* output)
{
   output[0] = input;
}
__kernel void kernelUShort(
   ushort2 input,
   __global ushort2* output)
{
   output[0] = input;
}

__kernel void kernelInt(
   int2 input,
   __global int2* output)
{
   output[0] = input;
}
__kernel void kernelUInt(
   uint2 input,
   __global uint2* output)
{
   output[0] = input;
}

__kernel void kernelLong(
   long2 input,
   __global long2* output)
{
   output[0] = input;
}
__kernel void kernelULong(
   ulong2 input,
   __global ulong2* output)
{
   output[0] = input + 4294967297;
}

__kernel void kernelFloat(
   float2 input,
   __global float2* output)
{
   output[0] = input;
}
