__kernel void kernelChar(
   char16 input,
   __global char16* output)
{
   output[0] = input;
}
__kernel void kernelUChar(
   uchar16 input,
   __global uchar16* output)
{
   output[0] = input;
}

__kernel void kernelShort(
   short16 input,
   __global short16* output)
{
   output[0] = input;
}
__kernel void kernelUShort(
   ushort16 input,
   __global ushort16* output)
{
   output[0] = input;
}

__kernel void kernelInt(
   int16 input,
   __global int16* output)
{
   output[0] = input;
}
__kernel void kernelUInt(
   uint16 input,
   __global uint16* output)
{
   output[0] = input;
}

__kernel void kernelLong(
   long16 input,
   __global long16* output)
{
   output[0] = input;
}
__kernel void kernelULong(
   ulong16 input,
   __global ulong16* output)
{
   output[0] = input + 4294967297;
}

__kernel void kernelFloat(
   float16 input,
   __global float16* output)
{
   output[0] = input;
}
