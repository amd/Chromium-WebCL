__kernel void kernelChar(
   char3 input,
   __global char3* output)
{
   output[0] = input;
}
__kernel void kernelUChar(
   uchar3 input,
   __global uchar3* output)
{
   output[0] = input;
}

__kernel void kernelShort(
   short3 input,
   __global short3* output)
{
   output[0] = input;
}
__kernel void kernelUShort(
   ushort3 input,
   __global ushort3* output)
{
   output[0] = input;
}

__kernel void kernelInt(
   int3 input,
   __global int3* output)
{
   output[0] = input;
}
__kernel void kernelUInt(
   uint3 input,
   __global uint3* output)
{
   output[0] = input;
}

__kernel void kernelLong(
   long3 input,
   __global long3* output)
{
   output[0] = input;
}
__kernel void kernelULong(
   ulong3 input,
   __global ulong3* output)
{
   output[0] = input + 4294967297;
}

__kernel void kernelFloat(
   float3 input,
   __global float3* output)
{
   output[0] = input;
}
