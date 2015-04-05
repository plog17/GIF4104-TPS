__kernel
void convolve(__global unsigned char* inputImage, __constant float* inputFilter, __global unsigned char* output,const int imWidth, const int filterSize){

    int filterHalfSize = filterSize>>1;

    int i = get_group_id(0);
    int j = get_group_id(1); //Identification of work-item

    //Identification of work-item
    int idX = get_local_id(0);
    int idY = get_local_id(1);

    int x = i*BLOCK_SIZE + idX;
    int y = j*BLOCK_SIZE + idY;

    //Goes up to 15x15 filters
    __local float localBlockR[BLOCK_SIZE+14][BLOCK_SIZE+14];
    __local float localBlockG[BLOCK_SIZE+14][BLOCK_SIZE+14];
    __local float localBlockB[BLOCK_SIZE+14][BLOCK_SIZE+14];
    localBlockR[idX][idY] = inputImage[y*imWidth*4 + x*4];
    localBlockG[idX][idY] = inputImage[y*imWidth*4 + x*4 + 1];
    localBlockB[idX][idY] = inputImage[y*imWidth*4 + x*4 + 2];
    if (idX < filterSize)
    {
        int newx = x + BLOCK_SIZE;
        int newy = y;
        localBlockR[idX + BLOCK_SIZE][idY] = inputImage[newy*imWidth*4 + newx*4];
        localBlockG[idX + BLOCK_SIZE][idY] = inputImage[newy*imWidth*4 + newx*4 + 1];
        localBlockB[idX + BLOCK_SIZE][idY] = inputImage[newy*imWidth*4 + newx*4 + 2];
    }
    if (idY < filterSize)
    {
        int newx = x;
        int newy = y + BLOCK_SIZE;
        localBlockR[idX][idY + BLOCK_SIZE] = inputImage[newy*imWidth*4 + newx*4];
        localBlockG[idX][idY + BLOCK_SIZE] = inputImage[newy*imWidth*4 + newx*4 + 1];
        localBlockB[idX][idY + BLOCK_SIZE] = inputImage[newy*imWidth*4 + newx*4 + 2];
    }
    if(idX < filterSize && idY < filterSize){
        int newx = x + BLOCK_SIZE;
        int newy = y + BLOCK_SIZE;
        localBlockR[idX + BLOCK_SIZE][idY + BLOCK_SIZE] = inputImage[newy*imWidth*4 + newx*4];
        localBlockG[idX + BLOCK_SIZE][idY + BLOCK_SIZE] = inputImage[newy*imWidth*4 + newx*4 + 1];
        localBlockB[idX + BLOCK_SIZE][idY + BLOCK_SIZE] = inputImage[newy*imWidth*4 + newx*4 + 2];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    int4 rgbValues = (int4)(0,0,0,0);
    for (int yFilter = 0; yFilter <= filterSize; yFilter++) {
        for (int xFilter = 0; xFilter <= filterSize; xFilter++) {
            rgbValues.x = mad(inputFilter[xFilter + yFilter*filterSize], localBlockR[idX+xFilter][idY+yFilter], rgbValues.x);
            rgbValues.y = mad(inputFilter[xFilter + yFilter*filterSize], localBlockG[idX+xFilter][idY+yFilter], rgbValues.y);
            rgbValues.z = mad(inputFilter[xFilter + yFilter*filterSize], localBlockB[idX+xFilter][idY+yFilter], rgbValues.z);
        }
    }

    int outputX = x + filterHalfSize;
    int outputY = y + filterHalfSize;
    output[outputY*imWidth*4 + outputX*4] = convert_char(rgbValues.x);
    output[outputY*imWidth*4 + outputX*4 + 1] = convert_char(rgbValues.y);
    output[outputY*imWidth*4 + outputX*4 + 2] = convert_char(rgbValues.z);
    output[outputY*imWidth*4 + outputX*4 + 3] = 255;
}
