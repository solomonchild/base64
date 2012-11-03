#include <stdio.h>
#include <stdlib.h>

char* unpacked_bytes;

int
get_next_4_bytes(FILE*);

void
unpack(int packed);

void
decode(FILE* fp);

int
main(int argc, char** argv)
{
	if(argc == 1)
	{
		printf("Error parsing command. Usage: base64_dec [filename].");
		return 1;
	}
	unpacked_bytes = calloc(4, sizeof(char));
	FILE* fp;
	fp = fopen(argv[1],"r");
	if(fp == NULL)
	{
		printf("Error opening file \"%s\"",argv[1]);
		return 1;
	}
	decode(fp);
	free(unpacked_bytes);
}

int
get_next_4_bytes(FILE* fp)
{
  int i;
  int temp_char;
  int res;
  for(i = 0; i<4; i++)
  {
	temp_char = fgetc(fp);
	if(temp_char == EOF)
	  return EOF;
	if(temp_char>=65&& temp_char<=90)
	  temp_char = temp_char-65;
	else if(temp_char>96 && temp_char<=122)
	  temp_char = temp_char-71;
	else if(temp_char>47 && temp_char<=57)
	  temp_char = temp_char+4;
	printf("Took 1 char of file: %c\n", temp_char);
	res <<= 8;
	res |= temp_char;
	printf("res 6 bits: :%d\n",res&0x3F);
  }
  return res;
}

void
unpack(int packed)
{
	int i;
	int temp;
	char next_move; 
//	printf("Packed first six from right: %d\n", packed & 0x3F);
	for(i=0; i <= 2; i++)
	{
		next_move = (24 - 10*i)%12 + 2;
		temp = packed >>(10*i);
		int dropped_bytes = 0x3F >> (next_move - 2);
		printf("Next Move: %d\n",next_move);
		//printf("%d) Unpackedd: %d\n",i,;	
		unpacked_bytes[i] = ( ((temp >> 2) & ~dropped_bytes)|( (temp & (0x3F >> (next_move-2))) ) ) & 0xFF;	 
}
}
void decode(FILE* fp)
{
	int packed = get_next_4_bytes(fp);
	FILE* to_write = fopen("decoded.txt","w");
	while(packed!=EOF)
	{
		unpack(packed);
		int i;
		for(i = 2; i >= 0; i--)
			fputc(unpacked_bytes[i],to_write);
		packed = get_next_4_bytes(fp);
		printf("GOT\n");
	}
	close(to_write);
}
