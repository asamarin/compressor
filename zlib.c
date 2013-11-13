#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>

typedef unsigned char u_char;

void zlib_C (const char * in) {
	FILE * file_in = fopen (in, "rb");
	FILE * file_out = fopen ("zlib.out", "wb");
	u_char* buf_in;
	u_char* buf_out;
	long usrcLen[1], cdestLen;
	
	/* El tamanyo del fichero de entrada se almacena en usrcLen */
   fseek (file_in, 0, SEEK_END);
   usrcLen[0] = ftell (file_in);
   rewind (file_in);

   /* Leemos el fichero en buf_in */
	buf_in = (u_char *) malloc (sizeof(u_char) * usrcLen[0]);
   fread ((char *) buf_in, 1, usrcLen[0], file_in);

	/* Los datos comprimidos se almacenan en buf_out */
   buf_out = (u_char *) malloc (sizeof(u_char) * compressBound(usrcLen[0]));
   compress ((Bytef *) buf_out, (uLongf *) &cdestLen, (Bytef *) buf_in, usrcLen[0]);

	/* Escribimos en los 4 primeros bytes el tamanyo del ficheo original */
	fwrite (usrcLen, sizeof(long), 1, file_out);
	/* Escribimos ahora el fichero comprimido */
	fwrite (buf_out, 1, cdestLen, file_out);
 
	/* Cerramos fichero y liberamos buffers */
	fclose (file_in);
   fclose (file_out);
	free (buf_in);
	free (buf_out);
}

/* -------------------------------------------------------- */

void zlib_D (const char * in) {
	FILE * file_in = fopen (in, "rb");
	FILE * file_out = fopen ("zlib.orig", "wb");
	u_char *buf_in, *buf_out;
	long csrcLen, udestLen[1];

	/* csrcLen contiene el tamanyo del fichero comprimido */
	fseek (file_in, 0, SEEK_END);
   csrcLen = ftell (file_in);
   rewind (file_in);
	
	/* Leemos los primeros 4 bytes, que corresponden a la longitud del 
	 * fichero original
	 */
   fread (udestLen, sizeof(long), 1, file_in);

	/* Decrementamos por tanto 4 bytes a lo que queda de fichero por leer */
	csrcLen -= sizeof(long);

	/* Reservamos memoria para el buffer de lectura y leemos */
	buf_in = (u_char *) malloc (sizeof(u_char) * csrcLen);
   fread ((char *) buf_in, 1, csrcLen, file_in);

	/* Sabiendo lo que ocupaba el fichero original, allocatamos
	 * el buffer convenientemente
	 */
	buf_out = (u_char *) malloc (sizeof(u_char) * udestLen[0]);

	/* Descomprimimos (buf_out contendra el fichero original descomprimido */
	uncompress ((Bytef *) buf_out, (uLongf *) &udestLen, (Bytef *) buf_in, csrcLen);

	/* Lo escribimos en fichero */
   fwrite (buf_out, 1, udestLen[0], file_out);
	
	/* Cerramos los ficheros abiertos y liberamos buffers */
	fclose (file_in);
   fclose (file_out);
	free (buf_in);
	free (buf_out);

}

/* -------------------------------------------------------- */

int main (int argc, char *argv[]) {
   if (argc != 3) {
		fprintf (stderr, "Argumentos no validos");
		return 1;
	} else {
		if (!strcmp(argv[1], "-c")) {
			zlib_C (argv[2]);
		} else if (!strcmp(argv[1], "-d")) {
			zlib_D (argv[2]);
		} else {
			fprintf (stderr, "Argumento no reconocido: %s", argv[1]);
			return 1;
		}
	}
	return 0;
}
