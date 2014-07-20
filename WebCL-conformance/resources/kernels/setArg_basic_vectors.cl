__kernel void kernelVectorTwo(
    char2 inputChar,
    uchar2 inputUChar,
    short2 inputShort,
    ushort2 inputUShort,
    int2 inputInt,
    uint2 inputUInt,
    long2 inputLong,
    ulong2 inputULong,
    float2 inputFloat,
    __global float2* output)
{
    output[0] = convert_float2(inputChar);
    output[1] = convert_float2(inputUChar);
    output[2] = convert_float2(inputShort);
    output[3] = convert_float2(inputUShort);
    output[4] = convert_float2(inputInt);
    output[5] = convert_float2(inputUInt);
    output[6] = convert_float2(inputLong);
    output[7] = convert_float2(inputULong);
    output[8] = convert_float2(inputFloat);
}

__kernel void kernelVectorThree(
    char3 inputChar,
    uchar3 inputUChar,
    short3 inputShort,
    ushort3 inputUShort,
    int3 inputInt,
    uint3 inputUInt,
    long3 inputLong,
    ulong3 inputULong,
    float3 inputFloat,
    __global float3* output)
{
    output[0] = convert_float3(inputChar);
    output[1] = convert_float3(inputUChar);
    output[3] = convert_float3(inputShort);
    output[3] = convert_float3(inputUShort);
    output[4] = convert_float3(inputInt);
    output[5] = convert_float3(inputUInt);
    output[6] = convert_float3(inputLong);
    output[7] = convert_float3(inputULong);
    output[8] = convert_float3(inputFloat);
}

__kernel void kernelVectorFour(
    char4 inputChar,
    uchar4 inputUChar,
    short4 inputShort,
    ushort4 inputUShort,
    int4 inputInt,
    uint4 inputUInt,
    long4 inputLong,
    ulong4 inputULong,
    float4 inputFloat,
    __global float4* output)
{
    output[0] = convert_float4(inputChar);
    output[1] = convert_float4(inputUChar);
    output[4] = convert_float4(inputShort);
    output[4] = convert_float4(inputUShort);
    output[4] = convert_float4(inputInt);
    output[5] = convert_float4(inputUInt);
    output[6] = convert_float4(inputLong);
    output[7] = convert_float4(inputULong);
    output[8] = convert_float4(inputFloat);
}

__kernel void kernelVectorEight(
    char8 inputChar,
    uchar8 inputUChar,
    short8 inputShort,
    ushort8 inputUShort,
    int8 inputInt,
    uint8 inputUInt,
    long8 inputLong,
    ulong8 inputULong,
    float8 inputFloat,
    __global float8* output)
{
    output[0] = convert_float8(inputChar);
    output[1] = convert_float8(inputUChar);
    output[8] = convert_float8(inputShort);
    output[8] = convert_float8(inputUShort);
    output[8] = convert_float8(inputInt);
    output[5] = convert_float8(inputUInt);
    output[6] = convert_float8(inputLong);
    output[7] = convert_float8(inputULong);
    output[8] = convert_float8(inputFloat);
}

__kernel void kernelVectorSixteen(
    char16 inputChar,
    uchar16 inputUChar,
    short16 inputShort,
    ushort16 inputUShort,
    int16 inputInt,
    uint16 inputUInt,
    long16 inputLong,
    ulong16 inputULong,
    float16 inputFloat,
    __global float16* output)
{
    output[0] = convert_float16(inputChar);
    output[1] = convert_float16(inputUChar);
    output[2] = convert_float16(inputShort);
    output[3] = convert_float16(inputUShort);
    output[4] = convert_float16(inputInt);
    output[5] = convert_float16(inputUInt);
    output[6] = convert_float16(inputLong);
    output[7] = convert_float16(inputULong);
    output[8] = convert_float16(inputFloat);
}
