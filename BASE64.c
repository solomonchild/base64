#include <stdio.h>
#include <stdlib.h>


int
encode(FILE*);

char* unpacked_chunk;

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		printf("Error parsing command.Usage: base64_enc [filename]");
		return 1;
	}

    unpacked_chunk = calloc(4,sizeof(char));
    FILE* fp;
    fp = fopen(argv[1],"r");
    
    if(fp==NULL)
    {
        printf("Failed to open a file \"%s\".",argv[1]);
        return 1;
    }

    encode(fp);
    free(unpacked_chunk);
    return 0;
}

int
get_next_3_bytes(FILE* fp, int* len_of_unpacked)
{
     
    int acc = 0;
    int temp = 0;
    int i;
    for(i=0;i<3;i++)
    {
	  temp = fgetc(fp); 
	  //10 is a LF in Unix.	
	  if(temp==EOF||temp==10)
	  {
		if(i==0)
		  return EOF;
		temp = 0;
		*len_of_unpacked=i;
		//printf("len of unp. in returns: %d\n",*len_of_unpacked);	
		break;	
	  }
	  (*len_of_unpacked)++;
	  acc<<=8;
	  acc|=temp;
	}
	return acc;
}



void
unpack(int chunk,int* len_of_unpacked)
{
  int i;
  if(*len_of_unpacked==1)
  {
	chunk<<=4;
	//printf("shiftl 4\n");
  }
  else if(*len_of_unpacked==2)
	chunk <<= 2;
  for(i=*len_of_unpacked; i >=0; i--)
  {
	unpacked_chunk[i] = (char)(chunk >> (6 * (*len_of_unpacked-i)) & 0x3F);
  }	
}

int
encode(FILE* fp)
{
  FILE* to_write;
  int temp_len;
  int packed_chunk;
  int len_of_unpacked = 0;
  to_write = fopen("encoded.txt","w+");
  packed_chunk = get_next_3_bytes(fp, &len_of_unpacked);
  while(packed_chunk!=EOF)
  {
	unpack(packed_chunk,&len_of_unpacked);
	//printf("\n LEN: %d \n",len_of_unpacked);
	int i; 
	for(i = 0;i<=len_of_unpacked;i++)
	{
	  int current = unpacked_chunk[i];
	  //printf("%d) %d\n",i,current);
	  //printf("%d\n: ",current);
	  if(current>=0 && current<=25)
		fputc(current+65,to_write);
	  else if(current>25 && current<=51)
		fputc(current+71,to_write);
	  else if(current>51 && current<=61)
		fputc(current-4,to_write);
	  else if(current == 62)
		fputc(43,to_write);
	  else
		fputc(47,to_write);
	}
	temp_len = len_of_unpacked;
	len_of_unpacked = 0;
	packed_chunk = get_next_3_bytes(fp, &len_of_unpacked);
  }
  // printf("before write len_of_unp: %d\n",len_of_unpacked);
  if(temp_len==2)
	fputs("=",to_write);
  else if(temp_len==1)
	fputs("==",to_write);
  fflush(to_write);
  fclose(to_write);
  return 0;
}

