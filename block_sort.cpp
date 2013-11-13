/* BLOCK SORTING - ETAPA DE COMPRESION 
 *
 * Se incluye aqui una implementacion, aun en pruebas,
 * del algoritmo Block-Sorting de Burrows & Wheeler. Dicha
 * implementacion ha sido desarrollada integramente por el
 * autor de este codigo (Alejandro Samarin Perez), y en principio
 * realiza en tiempo computacional lineal las rotaciones necesarias
 * mediante Radix-Sorting modificado.
 *
 * USO: $> make block_sort
 *      $> ./block_sort 'fich_in' > 'fich_out' 
 */

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <fstream>

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

using namespace std;

/* -------------------------------------------------- */

static inline
vector<u_char> build_L (vector<u_short> &V, const u_char* S, const size_t len) {
   vector<u_char> L;
   for (size_t i = 0; i < len; i++) {
      if (!V[i]) { 
         L.push_back (S[len - 1]);
      } else {
         L.push_back (S[V[i] - 1]);
      }
   }
   return L;
}

/* -------------------------------------------------- */

static inline
vector<u_short> pop_all (map<u_char,queue<u_short> > &arr, u_short &I) {
   vector<u_short> ordenado;
   u_int j = 0;
   map<u_char,queue<u_short> >::iterator it;
   for (it = arr.begin(); it != arr.end(); it++) {
      while (!((*it).second).empty()) {
         u_short tmp = ((*it).second).front();
         ordenado.push_back(tmp);
         if (!tmp)
            I = j;
         ((*it).second).pop();
         j++;
      }
   }
   return ordenado;
}

/* -------------------------------------------------- */

static inline
pair<vector<u_char>, u_short> bs_C (const u_char* S, const size_t len) {
   size_t i;
   u_short I = 0;
   map<u_char,queue<u_short> > array;
   // Primera iteracion
   for (i = 0; i < (len - 1); i++)
      array[S[i+1]].push (i);
   array[S[0]].push (len - 1);
   vector<u_short> arr2 = pop_all(array, I);
   // Segunda iteracion
   for (i = 0; i < len; i++)
      array[S[arr2[i]]].push (arr2[i]);
   arr2 = pop_all(array, I);
   return make_pair(build_L(arr2, S, len), I);
}

/* -------------------------------------------------- */

int main (int argc, char *argv[]) 
{
   u_char* buffer;
   ifstream fich_in(argv[1], ios::binary);
   fich_in.seekg (0, ios::end);
   size_t len = fich_in.tellg();
   fich_in.seekg (0, ios::beg);
   buffer = new u_char [len];
   fich_in.read ((char *)buffer, len);
   fich_in.close();
   // Block-sorting
   pair<vector<u_char>, u_short> blk_sorted (bs_C(buffer, len));
   for (size_t i = 0; i < blk_sorted.first.size(); i++) cout << blk_sorted.first[i];
   cout << endl;
   cout << "I = " << blk_sorted.second << endl;
   delete [] buffer;
   return 0;
}

