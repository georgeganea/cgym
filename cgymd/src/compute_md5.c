#include <openssl/md5.h>
#include <stdio.h>

long compute_md5(char* filename){
	FILE *file;
	MD5_CTX context;
	int len;
	unsigned char buffer[1024], digest[16];

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
}

void MD5Print (unsigned char digest[16]){

  unsigned int i;

  for (i = 0; i < 16; i++)
	  printf ("%02x", digest[i]);
}