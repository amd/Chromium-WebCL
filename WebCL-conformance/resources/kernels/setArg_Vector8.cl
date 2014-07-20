__kernel void kernelChar(
   char8 input,
   __global char8* output)
{
   output[0] = input;
}
__kernel void kernelUChar(
   uchar8 input,
   __global uchar8* output)
{
   output[0] = input;
}

__kernel void kernelShort(
   short8 input,
   __global short8* output)
{
   output[0] = input;
}
__kernel void kernelUShort(
   ushort8 input,
   __global ushort8* output)
{
   output[0] = input;
}

__kernel void kernelInt(
   int8 input,
   __global int8* output)
{
   output[0] = input;
}
__kernel void kernelUInt(
   uint8 input,
   __global uint8* output)
{
   output[0] = input;
}

__kernel void kernelLong(
   long8 input,
   __global long8* output)
{
   output[0] = input;
}
__kernel void kernelULong(
   ulong8 input,
   __global ulong8* output)
{
   output[0] = input + 4294967297;
}

__kernel void kernelFloat(
   float8 input,
   __global float8* output)
{
   output[0] = input;
}
