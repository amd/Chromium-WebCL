__kernel void kernelChar(
   char4 input,
   __global char4* output)
{
   output[0] = input;
}
__kernel void kernelUChar(
   uchar4 input,
   __global uchar4* output)
{
   output[0] = input;
}

__kernel void kernelShort(
   short4 input,
   __global short4* output)
{
   output[0] = input;
}
__kernel void kernelUShort(
   ushort4 input,
   __global ushort4* output)
{
   output[0] = input;
}

__kernel void kernelInt(
   int4 input,
   __global int4* output)
{
   output[0] = input;
}
__kernel void kernelUInt(
   uint4 input,
   __global uint4* output)
{
   output[0] = input;
}

__kernel void kernelLong(
   long4 input,
   __global long4* output)
{
   output[0] = input;
}
__kernel void kernelULong(
   ulong4 input,
   __global ulong4* output)
{
   output[0] = input + 4294967297;
}

__kernel void kernelFloat(
   float4 input,
   __global float4* output)
{
   output[0] = input;
}
