#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <cctype>
#include <unistd.h>

#define hibyte(x) (u_char)(x >> 8);
#define lobyte(x) (u_char)(x & 0xFF)

typedef unsigned char u_char;
typedef unsigned short u_short;

using namespace std;

/* -------------------------------------------------- */

// Sobrecarga para concatenar dos vectores
vector<u_char> operator+ (const vector<u_char> &v1, const vector<u_char> &v2) {
   vector<u_char> temp (v1);
   temp.insert (temp.end(), v2.begin(), v2.end());
   return temp;
}

/* -------------------------------------------------- */

// Usado en la descompresion de LZW
vector<u_char> operator+ (const vector<u_char> &v1, const u_char &c) {
   vector<u_char> temp (v1);
   temp.push_back (c);
   return temp;
}

/* -------------------------------------------------- */

// Inicializacion de las 256 primeras entradas en el diccionario del compresor
map<vector<u_char>,u_short> init_dict1 () {
   map<vector<u_char>,u_short> D;
   vector<u_char> tmp;
   u_char c;
   for (size_t i = 0; i <= UCHAR_MAX; i++) {
      c = (u_char) i;
      tmp.push_back (c);
      D[tmp] = i;
      tmp.clear();
   }
   return D;
}

/* -------------------------------------------------- */

// Inicializacion de las 256 primeras entradas en el diccionario del descompresor
map<u_short,vector<u_char> > init_dict2 () {
   map<u_short,vector<u_char> > D;
   vector<u_char> tmp;
   u_char c;
   for (size_t i = 0; i <= UCHAR_MAX; i++) {
      c = (u_char) i;
      tmp.push_back (c);
      D[i] = tmp;
      tmp.clear();
   }
   return D;
}

/* -------------------------------------------------- */

// Funcion principal de compresion
static inline 
void lzw_C (FILE * in) { 
   map<vector<u_char>,u_short> dict (init_dict1());
   map<vector<u_char>,u_short>::iterator it;
   u_short dict_index = UCHAR_MAX + 1;
   u_char c;
   vector<u_char> suf, str;
   vector<u_char> pref;
   vector<u_short> lzw_out;
   // Leemos primer byte y lo asignamos al prefijo
   c = fgetc (in);
   pref.push_back (c);
   c = fgetc (in);
   // Seguimos leyendo hasta fin de fichero
   while (!feof(in)) {
      suf.push_back (c);
      // 'str' se forma por la concatenacion de pref y suf
      str = pref + suf;
      it = dict.find(str);
      // Si la cadena 'str' no se encontro en el diccionario...
      if (it == dict.end()) { 
         // ... escribimos en el vector de salida el indice del prefijo...
         lzw_out.push_back(dict[pref]);
         pref = suf;
         // ... y si es posible, anyadimos 'str' al diccionario
         if (dict_index != USHRT_MAX) {
            dict[str] = dict_index; 
            dict_index++;
         }
      } else {    // Si la cadena 'str' si estaba en el diccionario
         pref = str;
      }
      suf.clear();      // Limpiamos el sufijo
      c = fgetc (in);   // Leemos siguiente byte 
   }
   // Al salir del bucle introducimos el indice del prefijo
   lzw_out.push_back(dict[pref]);
   // Sacamos el vector de datos comprimidos a la salida
   for (size_t i = 0; i < lzw_out.size(); i++) {
      cout << hibyte(lzw_out[i]);
      cout << lobyte(lzw_out[i]);
   }
   fclose (in);
}

/* -------------------------------------------------- */

// Funcion principal de descompresion
// NOTA: La notacion usada para las variables esta basada parcialmente en 
// la que usa Wikipedia (http://en.wikipedia.org/wiki/Lempel-Ziv-Welch#Algorithm)
static inline
void lzw_D (FILE * in) { 
  	map<u_short,vector<u_char> > dict (init_dict2());
   map<u_short,vector<u_char> >::iterator it;
   vector<u_short> compressed;
   vector<u_char> w, entry, lzw_out;
  	u_short index, k, dict_index = UCHAR_MAX + 1;
   u_char c;

   // Coger primer indice (ocupa los primeros dos bytes)
	c = fgetc (in);
   index = ((u_short) (c << 8));	// Byte alto del indice
   c = fgetc (in);
   index |= ((u_short) c);       // Byte bajo
   // Bucle para coger el resto de indices hasta fin de fichero
	while (!feof(in)) {
		// Introducimos el indice leido en el vector de indices
      compressed.push_back (index);
		// Obtenemos siguiente par de bytes
		c = fgetc (in);
      index = ((u_short) (c << 8));
      c = fgetc (in);
      index |= ((u_short) c);
	}
	// Comienzo del algoritmo
	k = compressed[0]; 	// Primer indice
   w.push_back ((dict[k])[0]);
   lzw_out.push_back (w[0]);
   for (size_t i = 1; i < compressed.size(); i++) { 
      k = compressed[i];
      it = dict.find (k);
      if (it != dict.end()) { // Si el indice k existe en el diccionario
         entry = dict[k];
      } else {
         entry = w + w[0];
      }
		// Concatenamos lo que contenga el vector 'entry' a la salida
		lzw_out.insert (lzw_out.end(), entry.begin(), entry.end());
      if (dict_index != USHRT_MAX) {
         dict[dict_index] = w + entry[0];
         dict_index++;
      }
      w = entry;
   }
   fclose (in);
   // Sacamos los datos descomprimidos a la salida
   for (size_t i = 0; i < lzw_out.size(); i++) { cout << lzw_out[i]; }
}

/* -------------------------------------------------- */

// Funcion para imprimir la ayuda
void help (const char* prog_name) {
	cerr << "Uso: " << prog_name;
	cerr << " [-c 'archivo_a_comprimir' | ";
 	cerr << "-d 'archivo_a_descomprimir' ]"; 
	cerr << " [ > 'archivo_de_salida' ]";
	cerr << endl;
}

/* -------------------------------------------------- */

int main (char argc, char *argv[]) 
{
	int arg, comp_flag = 0, uncomp_flag = 0;
   char *comp_file = NULL, *uncomp_file = NULL;
   FILE * file_in;
   while ((arg = getopt (argc, argv, "hc:d:")) != -1) {
		switch (arg) {
			case 'c':
				comp_flag = 1;
				comp_file = optarg;
				break;
			case 'd':
				uncomp_flag = 1;
				uncomp_file = optarg;
				break;
			case 'h':
				help (argv[0]);
				break;
			case '?':
				if ((optopt == 'c') || (optopt == 'd')) 
               fprintf (stderr, "La opcion -%c requiere un argumento\n", optopt);
				else if (isprint (optopt))
               fprintf (stderr, "Opcion desconocida: `-%c'.\n", optopt);
            else
               fprintf (stderr, "Caracter de opcion desconocido: `\\x%x'.\n", optopt);
            return 1;
         default:
         	help (argv[0]);
		}
	}
	
	if (comp_flag) {
		if ((file_in = fopen (comp_file, "rb")) == NULL) {
			fprintf (stderr, "No se pudo abrir el fichero %s para lectura\n", comp_file); 			
			return 1;
		}
		lzw_C (file_in);
	} else if (uncomp_flag) {
		if ((file_in = fopen (uncomp_file, "rb")) == NULL) {
			fprintf (stderr, "No se pudo abrir el fichero %s para lectura\n", comp_file); 			
			return 1;
		}
		lzw_D (file_in);
	}
   return 0;
}

