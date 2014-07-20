__kernel void kernelAllDatatypes(
    char inputChar,
    unsigned char inputUChar,
    short inputShort,
    unsigned short inputUShort,
    int inputInt,
    unsigned int inputUInt,
    long inputLong,
    unsigned long inputULong,
    float inputFloat,
    __global float* output)
{
    output[0] = convert_float(inputChar);
    output[1] = convert_float(inputUChar);
    output[2] = convert_float(inputShort);
    output[3] = convert_float(inputUShort);
    output[4] = convert_float(inputInt);
    output[5] = convert_float(inputUInt);
    output[6] = convert_float(inputLong);
    output[7] = convert_float(inputULong);
    output[8] = convert_float(inputFloat);
}
