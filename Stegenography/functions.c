#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

/* Union with bitfields of structure to play with bits for encoding n decoding purpose*/

union byte
{
    unsigned char encode;
    struct
    {
	unsigned char b0:1;
	unsigned char :7;
    }bits;

}uvar;
//---------------------------------------------------------------------------------------------------------------------------

//to get the size of secret file data and storing it as global variable
int file_size;

//---------------------------------------------------------------------------------------------------------------------------

/*---##################################### Function Definitions #####################################-----*/

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //  printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{

    fseek(fptr, 0l, SEEK_END);							//go to eof position
    //uint size =ftell(fptr);
    //printf("size=%u\n",size);
    //returns size of secret file size
    return (ftell(fptr));							//return poiter position
}
//---------------------------------------------------------CHECKING CAPACITY--------------------------------------------//

Status check_capacity(EncodeInfo *encInfo)
{
    uint image_size = get_image_size_for_bmp(encInfo->fptr_src_image); 	//function call to get bmp filesize

    uint secret_size = get_file_size(encInfo->fptr_secret);		//function call to get secret filesize
    if(secret_size > image_size)
	return e_failure;					//secret file has more capacity then failure
    else
	return e_success;					//secret file has less capacity then success
}
//----------------------------------------COPYING BMP FILE HEADER--------------------------------------------------------//

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    uint i;
    char ch;
    //as fptr is pointing to someother position,rewind it to start of file

    rewind(fptr_src_image);
    rewind(fptr_dest_image);

    for(i = 0;i < 54;i++)
    {
	ch = fgetc(fptr_src_image);
	fputc(ch,fptr_dest_image);
    }
    return e_success;
}
//--------------------------------------------------------------------------------------------------------------------------

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */


Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    //Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
	return e_failure;
    }
    else
    {
	printf("INFO: Opened %s\n", encInfo->src_image_fname);
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
	return e_failure;
    }
    else
    {
	printf("INFO: Opened %s\n",encInfo->secret_fname);
    }
    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

    if(encInfo->stego_image_fname == NULL)
    {
	encInfo->stego_image_fname = "stego_img.bmp";
	printf("INFO: Output file not mentioned, creating %s as default\n",encInfo->stego_image_fname);

	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	printf("INFO: Opened %s\n", encInfo->stego_image_fname);
    }
    else
    {
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	printf("INFO: Opened %s\n", encInfo->stego_image_fname);

    }   
    //Do Error handling
    printf("INFO: Done\n");
    // No failure return e_success*/
    return e_success;
}
//--------------------------------------------------------------------------------------------------------------------------

Status encode_magic_string(const char magic_string, EncodeInfo *encInfo)
{
    //point file pointer to 55th byte to perform encoding procedure(to store magic string)

    fseek(encInfo->fptr_src_image,54l,SEEK_SET);
    fseek(encInfo->fptr_stego_image,54l,SEEK_SET);

    uint i;
    unsigned char temp,bit;
    temp = magic_string;

    for(i = 0;i < 8;i++)
    {
	bit = !!(temp & 01);	//get lsb of temp
	fread(&uvar.encode, sizeof(char), 1, encInfo->fptr_src_image);

	//Do error handling
	if(ferror(encInfo->fptr_src_image) != 0)
	{
	    clearerr(encInfo->fptr_src_image);
	    return e_failure;
	}


	uvar.bits.b0 = bit; //lsb is store in bitfield of union

	fwrite(&uvar.encode, sizeof(char), 1, encInfo->fptr_stego_image);//copy encode data steged image file

	//Do error handling
	if(ferror(encInfo->fptr_src_image) != 0)
	{
	    clearerr(encInfo->fptr_src_image);
	    return e_failure;
	}
	//shift temp variable to get next lsb bit
	temp>>=1;
    }
    //on success
    return e_success;
}
//---------------------------------------------------------------------------------------------------------------------------

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    uint i,j, count = 0,temp;
    unsigned char bit;

    uint totalsize,size_of_extn;
    totalsize = strlen(file_extn);	//string length of secret.txt file

    for(i = 0;file_extn[i] != '.';i++);	//take count where "." is present

    size_of_extn = totalsize - i;	//obtain sizeof extn i.e 4
    temp = size_of_extn;			//assign it to temp variable

    for(i = 0;i < 32;i++)		//4 is an integer so run loop for sizeof(int) times
    {
	bit = !!(temp & 01);		//take lsb and store n steged images (take one bit n store it in one byte)

	fread(&uvar.encode,sizeof(char),1,encInfo->fptr_src_image);

	//Do error handling
	if(ferror(encInfo->fptr_src_image) != 0)
	{
	    clearerr(encInfo->fptr_src_image);
	    return e_failure;
	}

	uvar.bits.b0 = bit;

	fwrite(&uvar.encode,sizeof(char),1,encInfo->fptr_stego_image);

	//do error handling
	if(ferror(encInfo->fptr_src_image)!= 0)
	{
	    clearerr(encInfo->fptr_src_image);
	    return e_failure;
	}
	temp >>= 1;
    }

    //storing an entension in array
    char *str,k = 0;
    str = (char *)malloc(size_of_extn*sizeof(char));	//Dynamic memory allocation

    for(i = 0; *(file_extn + i)!= '.'; i++);		//checks for ".", n does nothing
    for(j = i; *(file_extn + j)!= '\0'; j++)		//storing .txt
    {
	str[k++] = file_extn[j];
    }
    str[k]='\0';					//making it as string as condition fails when it reaches null


    //storing entension to steged image
    for(i = 0;*(str + i) != '\0';i++)
    { 
	for(j = 0;j < 8;j++)
	{

	    bit = !!(str[i] & 01);	//get lsb

	    fread(&uvar.encode,sizeof(char),1,encInfo->fptr_src_image);
	    if(ferror(encInfo->fptr_src_image)!= 0)
	    {
		clearerr(encInfo->fptr_src_image);
		return e_failure;
	    }


	    uvar.bits.b0 = bit;	//store lsb in bit feild

	    fwrite(&uvar.encode,sizeof(char),1,encInfo->fptr_stego_image);//write it to steged image

	    //do error handling
	    if(ferror(encInfo->fptr_src_image) != 0)
	    {
		clearerr(encInfo->fptr_src_image);
		return e_failure;
	    }
	    str[i]>>=1;
	}
    }
    return e_success;
}
//-------------------------------------------------------------------------------------------------------------------------

Status encode_secret_file_size(uint file_size, EncodeInfo *encInfo)
{
    uint i,temp=file_size;
    unsigned char bit;

    for(i = 0;i < 32;i++)
    {
	bit = !!(temp & 01);

	fread(&uvar.encode,sizeof(char),1,encInfo->fptr_src_image);

	if(ferror(encInfo->fptr_src_image)!= 0)
	{
	    clearerr(encInfo->fptr_src_image);
	    return e_failure;
	}


	uvar.bits.b0 = bit;

	fwrite(&uvar.encode ,sizeof(char) , 1 , encInfo->fptr_stego_image);

	if(ferror(encInfo->fptr_src_image) != 0)
	{
	    clearerr(encInfo->fptr_src_image);
	    return e_failure;
	}
	temp >>= 1;
    }

}
//---------------------------------------------------------------------------------------------------------------------------

Status encode_secret_file_data( EncodeInfo *encInfo)
{
    unsigned char bit;
    uint i;
    int ch;
    rewind(encInfo->fptr_secret);
    while((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
	for(i = 0;i < 8;i++)
	{
	    bit = !!(ch & 01);
	    fread(&uvar.encode , sizeof(char) , 1 , encInfo->fptr_src_image);
	    if(ferror(encInfo->fptr_src_image)!= 0)
	    {
		clearerr(encInfo->fptr_src_image);
		return e_failure;
	    }


	    uvar.bits.b0 = bit;
	    fwrite(&uvar.encode, sizeof(char), 1, encInfo->fptr_stego_image);
	    if(ferror(encInfo->fptr_src_image)!= 0)
	    {
		clearerr(encInfo->fptr_src_image);
		return e_failure;
	    } 
	    ch >>= 1;

	}
    }
    return e_success;
}
//---------------------------------------------------------------------------------------------------------------------------

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    uint i;
    for(i=0;(ch=fgetc(fptr_src))!=EOF;i++)
    {
	fputc(ch,fptr_dest);
    }
    return e_success;
}
//------------------------------------ENCODING FUNCTION DEFINITIONS DONE-----------------------------------------------------

//###########################################*********ENCODING*********#########################################

void encoding(int argc , char *argv[])
{
    EncodeInfo encInfo;
    //command line arguments(take user input)
    encInfo.src_image_fname=argv[2];
    encInfo.secret_fname=argv[3];
    encInfo.stego_image_fname=argv[4]; 

    printf("INFO: Opening required files\n");
    //fuction call
    open_files(&encInfo);
   

    printf("INFO: ####Encoding Procedure Started####\n");

    printf("INFO: Checking for secret.txt size\n");
    //function call
    encInfo.size_secret_file = get_file_size(encInfo.fptr_secret);

    if(encInfo.size_secret_file == e_success)
    {
	printf("INFO: EMPTY\n");
	exit(1);
    }
    else
    {
	printf("INFO: Done.Not Empty\n");
    }

    printf("INFO: Checking for %s capacity to handle %s\n",encInfo.src_image_fname,encInfo.secret_fname);
    uint capacity = check_capacity(&encInfo);

    if(capacity == e_success)
    {
	printf("INFO: Done! Found OK...\n");
    }
    else
    {
	exit(1);
    }

    //function call
    uint copy = copy_bmp_header(encInfo.fptr_src_image, encInfo.fptr_stego_image);
    if(copy == e_success)
    {
	printf("INFO: Copying Image Header\n");
    }
    else
    {
	exit(1);
    }
    printf("INFO: Done\n");

    printf("INFO: Encode Magic String Signature\n");
    //function call
    if(( encode_magic_string('*',&encInfo))==e_failure)
    {
	exit(1);
    }
    else
    {
	printf("INFO: Done.\n");

    }


    printf("INFO: Encode Secret.txt extension\n");
    //function call
    if((encode_secret_file_extn(encInfo.secret_fname, &encInfo))== e_failure)
    {
	exit(1);
    }
    else
    {

	printf("INFO: Done.\n");
    }

    printf("INFO: Encode Secret.txt File Size\n");
    //get file size(secret text)
    uint file_size= get_file_size( encInfo.fptr_secret );

    if(( encode_secret_file_size( file_size, &encInfo )) == e_failure)
    {
	exit(1);
    }
    else
    {
	printf("INFO: Done.\n");
    }

    printf("INFO: Encode Secret.txt File Data\n");
    //function call
    if((encode_secret_file_data( &encInfo ))==e_failure)
    {
	exit(1);
    }
    else
    {
	printf("INFO: Done.\n");
    }
    printf("INFO: Copy Left Over Data\n");

    //function call
    if((copy_remaining_img_data( encInfo.fptr_src_image, encInfo.fptr_stego_image ))== e_failure)
    {
	exit(1);
    }
    else
    {
	printf("INFO: Done.\n");
    }
    printf("INFO: YUPPIEEEEE!!!!!!---###ENCODING DONE###---\n");

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------DECODING---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//funtion definitions for decoding
Status Open_files(DecodeInfo *decInfo)
{

    decInfo->fptr_stego_fname = fopen(decInfo->stego_image_fname,"r");

    if(decInfo->fptr_stego_fname == NULL)
    {
	perror("fopen");
	fprintf(stderr,"error:unable to open file %s\n",decInfo->stego_image_fname);
	return e_failure;
    }
    else
    {
	printf("INFO: Opened %s\n",decInfo->stego_image_fname);
    }
    if(decInfo->secret_fname == NULL)
    {
	decInfo->secret_fname= "decode.txt";
	printf("INFO: Output file is not mentioned ,creating %s as default\n",decInfo->secret_fname);

	decInfo->fptr_secret = fopen(decInfo->secret_fname,"w");
	printf("INFO: Opened %s\n",decInfo->secret_fname);
    }
    else
    {
	decInfo->fptr_secret = fopen(decInfo->secret_fname,"w");
	printf("INFO: opened %s\n",decInfo->secret_fname);
    }
    return e_success;
}
//---------------------------------------------------------------------------------------------------------------------------

Status decode_magic_string(DecodeInfo *decInfo)
{
    unsigned char ch1=0,ch2,bit;
    uint i;
    //move file pointer to 55th position so that we can decode
    fseek(decInfo->fptr_stego_fname,54l,SEEK_SET);

    for(i = 0;i < 8; i++)
    {
	ch1 >>= 1;//to put each lsb bit

	fread(&uvar.encode , sizeof(char), 1, decInfo->fptr_stego_fname);

	//get bit from steged image
	bit = uvar.bits.b0;
	bit <<= 7;//move to msb while decoding bcz it is stored in reverse order during encodin procedure

	ch2 = bit & 0x80;//get msb 
	ch1 |= ch2;
    }
    if(ch1 == '*')
	return  e_success;
    else
	return e_failure;

} 
//---------------------------------------------------------------------------------------------------------------------------

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    uint ch1 = 0,ch2,bit;
    uint i,j;

    for(i = 0;i < 32;i++) //decoding size of file extn i.e 4(int =>32bits)
    {
	ch1 >>= 1;

	fread(&uvar.encode,sizeof(char),1,decInfo->fptr_stego_fname);

	bit=uvar.bits.b0;
	bit<<=31;
	ch2 = bit & 0x80000000;
	ch1|=ch2;
    }
    if(ch1 == 4);
    else
	return e_failure;

    //extractino of extension
    char str1[10];
    char *str=".txt";

    for(i = 0;i < 4;i++)//run sizeof extn tyms
    {
	ch1 = 0;
	for(j = 0;j < 8;j++)
	{
	    ch1 >>= 1;

	    fread(&uvar.encode,sizeof(char),1,decInfo->fptr_stego_fname);

	    if((ferror(decInfo->fptr_stego_fname))!= 0)
	    {
		clearerr(decInfo->fptr_stego_fname);
		return e_failure;
	    }

	    bit = uvar.bits.b0;
	    bit <<= 7;
	    ch2 = bit& 0x80;
	    ch1 |= ch2;
	    str1[i] = ch1;
	}
    }
    str1[i] ='\0';
    if((strcmp(str1,".txt")) == 0)
    return e_success;
    else
	return e_failure;
#if 0
    for(i=0;i<4;i++)
    {
	printf("%c",str[i]);
	puts("");
    }

    for(i=0;i<4;i++)
    {
	if(str1[i]==str[i])
	    printf("yes\n");
	else
	    printf("no\n");
    }
#endif
}
//---------------------------------------------------------------------------------------------------------------------------

Status decode_secret_file_size(DecodeInfo *decInfo)
{

    uint i,secret_size =0, temp,bit;

    for(i=0;i<32;i++)
    {
	secret_size >>= 1;

	fread(&uvar.encode, sizeof(char),1, decInfo->fptr_stego_fname);

	if(ferror(decInfo->fptr_stego_fname)!= 0)
	{
	    clearerr(decInfo->fptr_stego_fname);
	    return e_failure;
	}

	bit = uvar.bits.b0;
	bit <<=31;
	temp = bit & 0x80000000;
	secret_size |= temp;
    }

    file_size = secret_size;//assign secret size to global value

    return e_success;

}
//--------------------------------------------------------------------------------------------------------------------------

Status decode_secret_file_data(DecodeInfo *decInfo)
{

    uint i,j;
    unsigned char ch1 =0, ch2,bit;

    for(i = 0;i < file_size; i++)
    {
	for(j = 0 ;j < 8; j++)
	{
	    ch1 >>= 1;

	    fread(&uvar.encode,sizeof(char),1, decInfo->fptr_stego_fname);

	    if(ferror(decInfo->fptr_stego_fname)!= 0)
	    {
		clearerr(decInfo->fptr_stego_fname);
		return e_failure;
	    }

	    bit = uvar.bits.b0;
	    bit <<= 7;
	    ch2 = bit & 0x80;
	    ch1 |= ch2;

	}
	//store the data to decode.txt from steged image(decoded data)
	fputc(ch1,decInfo->fptr_secret);

	//fwrite(&ch1,sizeof(char),1,decInfo->fptr_secret);

	if(ferror(decInfo->fptr_stego_fname)!=0)
	{
	    clearerr(decInfo->fptr_stego_fname);
	    return e_failure;
	}
    }
    return e_success;
}


//********************************************************decoding***********************************************************

void decoding(int argc , char *argv[])
{
    DecodeInfo decInfo;
    //take inputs from user(cmd line arguments)
    decInfo.stego_image_fname = argv[2];
    decInfo.secret_fname = argv[3];

    printf("INFO: ##Decoding procedure started##\n");
    printf("INFO: Opening required files\n");

    if((Open_files(&decInfo))==e_failure)
    {
	exit(1);
    }
    else
    {
	printf("INFO: Done ,Opened all required files\n");

    }

    printf("INFO: Decoding Magic String\n");
    if((decode_magic_string(&decInfo))==e_failure)
    {
	exit(1);
    }
    else
    {
	printf("INFO: Done\n");
    }

    printf("INFO: Decoding Output File Extension\n");
    if((decode_secret_file_extn(&decInfo))==e_failure)
    {
	puts("bin\n");
	exit(1);
    }
    else
    {
	printf("INFO: Done\n");
    }

    printf("INFO: Decoding File Size\n");
    if((decode_secret_file_size(&decInfo))==e_failure)
    {
	puts("bin\n");
	exit(1);
    }
    else
    {
	printf("INFO: Done\n");
    }

    printf("INFO: Decoding File Data\n");
    if((decode_secret_file_data(&decInfo))==e_failure)
    {
	exit(1);
    }
    else
    {
	printf("INFO: Done\n");
    }

    printf("INFO: UFF!!!!--## Decoding Done Successfully##--\n");
}
//---------------------------------------------------DONE DECODING-----------------------------------------------------------
