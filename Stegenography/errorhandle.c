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
    int choice = 0,flag=0;
    int i,j;
    char s[5];
    char * str=argv[2];
    int len = strlen(str);
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

	    //case 1: if(argc == 4 || argc == 5)
	    case 1: if(argc >= 4)
		    {
			//str[50]=argv[2];
			for(i=0;str[i]!='.';i++);
			flag=len-i;
			for(j=0;j<flag;j++)
			{
			    s[j]=str[i];
			    i++;
			}
			s[j]='\0';
			for(i=0;s[i]!='\0';i++)
			    printf("%c",s[i]);
			puts(s);
			if((strcmp(s,".bmp"))==0)
			{
			    printf("encodingargc , argc");
			}
			else
			{
			    printf("#./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file].\n");
			}
		    }	

		    else
		    {	
			printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file].\n");
		  // exit(1);
		    }


		    break;

	    case 2: if(argc == 3 || argc == 4)
		    {
			//str[50]=argv[2];
			for(i=0;str[i]!='.';i++);



			flag=len-i;
			for(j=0;j<flag;j++)
			{
			    s[j]=str[i];
			    i++;
			}
			s[j]='\0';
			for(i=0;s[i]!='\0';i++)
			    printf("%c",s[i]);
			puts(s);
			if((strcmp(s,".bmp"))==0)
			{
			    printf("decoding argc , argc");
			}
			else
			{
			    printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file].\n");
			}
		    }//decoding(argc, argv);
		    else
		    {
			printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file].\n");
		    }
		    break;
	    default:
		    print_msg();
		    break;

	}

    }
}
