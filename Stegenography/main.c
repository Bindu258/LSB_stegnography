#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "encode.h"
#include "types.h"

void print_msg(void)
{
    printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file].\n");
    printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file].\n");
}

int main(int argc, char * argv[])
{
    int choice = 0;

    if(argc == 1)
    {
	print_msg();
    }

    else
    {

	if((strcmp(argv[1],"-e")) == 0)
	    choice =1;

	if ((strcmp(argv[1],"-d"))==0)
	    choice =2;

	switch(choice)
	{

	    case 1: if(argc == 4 || argc == 5)
		    {
			if((error_handling(argc , argv))==e_failure)
			{
			    exit(1);
			}
			encoding(argc , argv);
		    }
		    else
			printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file].\n");
		    break;

	    case 2: if(argc == 3 || argc == 4)
		    {
			error_handling(argc , argv);
			decoding(argc, argv);
		    }
		    else
			printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file].\n");
		    break;
	    default:
		    print_msg();
		    break;

	}

    }
}
