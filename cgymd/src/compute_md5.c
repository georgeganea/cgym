#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>

char* compute_md5(char* filename){
	FILE *file;
	MD5_CTX context;
	int len;
	unsigned char buffer[1024], digest[16];
	char sir[3];
	char final[33];

	if ((file = fopen (filename, "rb")) == NULL)
		perror("fopen");
	else {
		MD5_Init (&context);
		while (len = fread (buffer, 1, 1024, file))
			MD5_Update (&context, buffer, len);
		MD5_Final (digest, &context);

		fclose (file);

		printf ("MD5 (%s) = ", filename);
		MD5Print (digest);
		printf ("\n");
	}
	int i;
	for(i=0;i<16;i++){
		if (sprintf(sir,"%02x",digest[i])<0)
			perror("sprintf");
		memcpy(final,sir,strlen(sir)+1);
	}
	printf("%s",final);
	
}

void MD5Print (unsigned char digest[16]){

  unsigned int i;

  for (i = 0; i < 16; i++)
	  printf ("%02x", digest[i]);
}