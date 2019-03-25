#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "encode.h"
Status error_handling(int argc, char * argv[])
{
    int i,j,flag = 0;
    char s[5];
    char * str=argv[2];
    int len = strlen(str);

			//str[50]=argv[2];
			for(i=0;str[i]!='.';i++);
			flag=len-i;
			for(j=0;j<flag;j++)
			{
			    s[j]=str[i];
			    i++;
			}
			s[j]='\0';
//			for(i=0;s[i]!='\0';i++)
//			    printf("%c",s[i]);
			if((strcmp(s,".bmp"))==0)
			{
			    printf("INFO: matched files names with extension\n");
			}
			else
			{
			    printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file].\n");
			exit(1);
			}

		
}
