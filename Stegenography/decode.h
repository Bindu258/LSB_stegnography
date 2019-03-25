#ifndef DECODE_H
#define DECODE_H
#include "types.h"
typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_fname;

    char *secret_fname;
    FILE *fptr_secret;
}
DecodeInfo;

void decoding(int argc,char *argv[]);

Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
#endif
