#include "Stemmer.h"

/* The main part of the stemming algorithm starts here.
*/

#define TRUE 1
#define FALSE 0

/* stemmer is a structure for a few local bits of data,
*/

struct stemmer {
   char * b;       /* buffer for word to be stemmed */
   int k;          /* offset to the end of the string */
   int j;          /* a general offset into the string */
};


/* Member b is a buffer holding a word to be stemmed. The letters are in
   b[0], b[1] ... ending at b[z->k]. Member k is readjusted downwards as
   the stemming progresses. Zero termination is not in fact used in the
   algorithm.

   Note that only lower case sequences are stemmed. Forcing to lower case
   should be done before stem(...) is called.


   Typical usage is:

       struct stemmer * z = create_stemmer();
       char b[] = "pencils";
       int res = stem(z, b, 6);
           /- stem the 7 characters of b[0] to b[6]. The result, res,
              will be 5 (the 's' is removed). -/
       free_stemmer(z);
*/


/* cons(z, i) is TRUE <=> b[i] is a consonant. ('b' means 'z->b', but here
   and below we drop 'z->' in comments.
*/

static int cons(struct stemmer * z, int i)
{  switch (z->b[i])
   {  case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;
      case 'y': return (i == 0) ? TRUE : !cons(z, i - 1);
      default: return TRUE;
   }
}

/* m(z) measures the number of consonant sequences between 0 and j. if c is
   a consonant sequence and v a vowel sequence, and <..> indicates arbitrary
   presence,

      <c><v>       gives 0
      <c>vc<v>     gives 1
      <c>vcvc<v>   gives 2
      <c>vcvcvc<v> gives 3
      ....
*/

static int m(struct stemmer * z)
{  int n = 0;
   int i = 0;
   int j = z->j;
   while(TRUE)
   {  if (i > j) return n;
      if (! cons(z, i)) break; i++;
   }
   i++;
   while(TRUE)
   {  while(TRUE)
      {  if (i > j) return n;
            if (cons(z, i)) break;
            i++;
      }
      i++;
      n++;
      while(TRUE)
      {  if (i > j) return n;
         if (! cons(z, i)) break;
         i++;
      }
      i++;
   }
}

/* vowelinstem(z) is TRUE <=> 0,...j contains a vowel */

static int vowelinstem(struct stemmer * z)
{
   int j = z->j;
   int i; for (i = 0; i <= j; i++) if (! cons(z, i)) return TRUE;
   return FALSE;
}

/* doublec(z, j) is TRUE <=> j,(j-1) contain a double consonant. */

static int doublec(struct stemmer * z, int j)
{
   char * b = z->b;
   if (j < 1) return FALSE;
   if (b[j] != b[j - 1]) return FALSE;
   return cons(z, j);
}

/* cvc(z, i) is TRUE <=> i-2,i-1,i has the form consonant - vowel - consonant
   and also if the second c is not w,x or y. this is used when trying to
   restore an e at the end of a short word. e.g.

      cav(e), lov(e), hop(e), crim(e), but
      snow, box, tray.

*/

static int cvc(struct stemmer * z, int i)
{  if (i < 2 || !cons(z, i) || cons(z, i - 1) || !cons(z, i - 2)) return FALSE;
   {  int ch = z->b[i];
      if (ch  == 'w' || ch == 'x' || ch == 'y') return FALSE;
   }
   return TRUE;
}

/* ends(z, s) is TRUE <=> 0,...k ends with the string s. */

static int ends(struct stemmer * z, char * s)
{  int length = s[0];
   char * b = z->b;
   int k = z->k;
   if (s[length] != b[k]) return FALSE; /* tiny speed-up */
   if (length > k + 1) return FALSE;
   if (memcmp(b + k - length + 1, s + 1, length) != 0) return FALSE;
   z->j = k-length;
   return TRUE;
}

/* setto(z, s) sets (j+1),...k to the characters in the string s, readjusting
   k. */

static void setto(struct stemmer * z, char * s)
{  int length = s[0];
   int j = z->j;
   memmove(z->b + j + 1, s + 1, length);
   z->k = j+length;
}

/* r(z, s) is used further down. */

static void r(struct stemmer * z, char * s) { if (m(z) > 0) setto(z, s); }

/* step1ab(z) gets rid of plurals and -ed or -ing. e.g.

       caresses  ->  caress
       ponies    ->  poni
       ties      ->  ti
       caress    ->  caress
       cats      ->  cat

       feed      ->  feed
       agreed    ->  agree
       disabled  ->  disable

       matting   ->  mat
       mating    ->  mate
       meeting   ->  meet
       milling   ->  mill
       messing   ->  mess

       meetings  ->  meet

*/

static void step1ab(struct stemmer * z)
{
   // ends function strings
   char ends_string_1[] = "\04" "sses";
   char ends_string_2[] = "\03" "ies";
   char ends_string_3[] = "\03" "eed";
   char ends_string_4[] = "\02" "ed";
   char ends_string_5[] = "\03" "ing";
   char ends_string_6[] = "\02" "at";
   char ends_string_7[] = "\02" "bl";
   char ends_string_8[] = "\02" "iz";

   // setto function strings
   char setto_string_1[] = "\01" "i";
   char setto_string_2[] = "\03" "ate";
   char setto_string_3[] = "\03" "ble";
   char setto_string_4[] = "\03" "ize";
   char setto_string_5[] = "\01" "e";

   char * b = z->b;
   if (b[z->k] == 's')
   {  if (ends(z, ends_string_1)) z->k -= 2; else
      if (ends(z, ends_string_2)) setto(z, setto_string_1); else
      if (b[z->k - 1] != 's') z->k--;
   }
   if (ends(z, ends_string_3)) { if (m(z) > 0) z->k--; } else
   if ((ends(z, ends_string_4) || ends(z, ends_string_5)) && vowelinstem(z))
   {  z->k = z->j;
      if (ends(z, ends_string_6)) setto(z, setto_string_2); else
      if (ends(z, ends_string_7)) setto(z, setto_string_3); else
      if (ends(z, ends_string_8)) setto(z, setto_string_4); else
      if (doublec(z, z->k))
      {  z->k--;
         {  int ch = b[z->k];
            if (ch == 'l' || ch == 's' || ch == 'z') z->k++;
         }
      }
      else if (m(z) == 1 && cvc(z, z->k)) setto(z, setto_string_5);
   }
}

/* step1c(z) turns terminal y to i when there is another vowel in the stem. */

static void step1c(struct stemmer * z)
{
   char s[] = "\01" "y";
   if (ends(z, s) && vowelinstem(z)) z->b[z->k] = 'i';
}


/* step2(z) maps double suffices to single ones. so -ization ( = -ize plus
   -ation) maps to -ize etc. note that the string before the suffix must give
   m(z) > 0. */

static void step2(struct stemmer * z) 
{
   char ational[] = "\07" "ational";
   char tional[] = "\06" "tional";
   char enci[] = "\04" "enci";
   char anci[] = "\04" "anci";
   char izer[] = "\04" "izer";
   char bli[] = "\03" "bli";
   char alli[] = "\04" "alli";
   char entli[] = "\05" "entli";
   char eli[] = "\03" "eli";
   char ousli[] = "\05" "ousli";
   char ization[] = "\07" "ization";
   char ation[] = "\05" "ation";
   char ator[] = "\04" "ator";
   char alism[] = "\05" "alism";
   char iveness[] = "\07" "iveness";
   char fulness[] = "\07" "fulness";
   char ousness[] = "\07" "ousness";
   char aliti[] = "\05" "aliti";
   char iviti[] = "\05" "iviti";
   char biliti[] = "\06" "biliti";
   char logi[] = "\04" "logi";
   char ate[] = "\03" "ate";
   char tion[] = "\04" "tion";
   char ence[] = "\04" "ence";
   char ance[] = "\04" "ance";
   char ize[] = "\03" "ize";
   char ble[] = "\03" "ble";
   char al[] = "\02" "al";
   char ent[] = "\03" "ent";
   char e[] = "\01" "e";
   char ous[] = "\03" "ous";
   char ive[] = "\03" "ive";
   char ful[] = "\03" "ful";
   char log[] = "\03" "log";

   switch (z->b[z->k-1]){
   case 'a': if (ends(z, ational)) { r(z, ate); break; }
             if (ends(z, tional)) { r(z, tion); break; }
             break;
   case 'c': if (ends(z, enci)) { r(z, ence); break; }
             if (ends(z, anci)) { r(z, ance); break; }
             break;
   case 'e': if (ends(z, izer)) { r(z, ize); break; }
             break;
   case 'l': if (ends(z, bli)) { r(z, ble); break; } /*-DEPARTURE-*/

 /* To match the published algorithm, replace this line with
    case 'l': if (ends(z, "\04" "abli")) { r(z, "\04" "able"); break; } */

             if (ends(z, alli)) { r(z, al); break; }
             if (ends(z, entli)) { r(z, ent); break; }
             if (ends(z, eli)) { r(z, e); break; }
             if (ends(z, ousli)) { r(z, ous); break; }
             break;
   case 'o': if (ends(z, ization)) { r(z, ize); break; }
             if (ends(z, ation)) { r(z, ate); break; }
             if (ends(z, ator)) { r(z, ate); break; }
             break;
   case 's': if (ends(z, alism)) { r(z, al); break; }
             if (ends(z, iveness)) { r(z, ive); break; }
             if (ends(z, fulness)) { r(z, ful); break; }
             if (ends(z, ousness)) { r(z, ous); break; }
             break;
   case 't': if (ends(z, aliti)) { r(z, al); break; }
             if (ends(z, iviti)) { r(z, ive); break; }
             if (ends(z, biliti)) { r(z, ble); break; }
             break;
   case 'g': if (ends(z, logi)) { r(z, log); break; } /*-DEPARTURE-*/

 /* To match the published algorithm, delete this line */

} }

/* step3(z) deals with -ic-, -full, -ness etc. similar strategy to step2. */

static void step3(struct stemmer * z) { 
   char icate[] = "\05" "icate";
   char ative[] = "\05" "ative";
   char alize[] = "\05" "alize";
   char iciti[] = "\05" "iciti";
   char ical[] = "\04" "ical";
   char ful[] = "\03" "ful";
   char ness[] = "\04" "ness";
   char ic[] = "\02" "ic";
   char blank[] = "\00" "";
   char al[] = "\02" "al";

   switch (z->b[z->k])
{
   case 'e': if (ends(z, icate)) { r(z, ic); break; }
             if (ends(z, ative)) { r(z, blank); break; }
             if (ends(z, alize)) { r(z, al); break; }
             break;
   case 'i': if (ends(z, iciti)) { r(z, ic); break; }
             break;
   case 'l': if (ends(z, ical)) { r(z, ic); break; }
             if (ends(z, ful)) { r(z, blank); break; }
             break;
   case 's': if (ends(z, ness)) { r(z, blank); break; }
             break;
} }

/* step4(z) takes off -ant, -ence etc., in context <c>vcvc<v>. */

static void step4(struct stemmer * z)
{  
   char al[] = "\02" "al";
   char ance[] = "\04" "ance";
   char ence[] = "\04" "ence";
   char er[] = "\02" "er";
   char ic[] = "\02" "ic";
   char able[] = "\04" "able";
   char ible[] = "\04" "ible";
   char ant[] = "\03" "ant";
   char ement[] = "\05" "ement";
   char ment[] = "\04" "ment";
   char ent[] = "\03" "ent";
   char ou[] = "\02" "ou";
   char ion[] = "\03" "ion";
   char ism[] = "\03" "ism";
   char ate[] = "\03" "ate";
   char iti[] = "\03" "iti";
   char ous[] = "\03" "ous";
   char ive[] = "\03" "ive";
   char ize[] = "\03" "ize";

   switch (z->b[z->k-1])
   {  case 'a': if (ends(z, al)) break; return;
      case 'c': if (ends(z, ance)) break;
                if (ends(z, ence)) break; return;
      case 'e': if (ends(z, er)) break; return;
      case 'i': if (ends(z, ic)) break; return;
      case 'l': if (ends(z, able)) break;
                if (ends(z, ible)) break; return;
      case 'n': if (ends(z, ant)) break;
                if (ends(z, ement)) break;
                if (ends(z, ment)) break;
                if (ends(z, ent)) break; return;
      case 'o': if (ends(z, ion) && z->j >= 0 && (z->b[z->j] == 's' || z->b[z->j] == 't')) break;
                if (ends(z, ou)) break; return;
                /* takes care of -ous */
      case 's': if (ends(z, ism)) break; return;
      case 't': if (ends(z, ate)) break;
                if (ends(z, iti)) break; return;
      case 'u': if (ends(z, ous)) break; return;
      case 'v': if (ends(z, ive)) break; return;
      case 'z': if (ends(z, ize)) break; return;
      default: return;
   }
   if (m(z) > 1) z->k = z->j;
}

/* step5(z) removes a final -e if m(z) > 1, and changes -ll to -l if
   m(z) > 1. */

static void step5(struct stemmer * z)
{
   char * b = z->b;
   z->j = z->k;
   if (b[z->k] == 'e')
   {  int a = m(z);
      if (a > 1 || a == 1 && !cvc(z, z->k - 1)) z->k--;
   }
   if (b[z->k] == 'l' && doublec(z, z->k) && m(z) > 1) z->k--;
}

/* In stem(z, b, k), b is a char pointer, and the string to be stemmed is
   from b[0] to b[k] inclusive.  Possibly b[k+1] == '\0', but it is not
   important. The stemmer adjusts the characters b[0] ... b[k] and returns
   the new end-point of the string, k'. Stemming never increases word
   length, so 0 <= k' <= k.
*/

extern int stem(struct stemmer * z, char * b, int k)
{
   if (k <= 1) return k; /*-DEPARTURE-*/
   z->b = b; z->k = k; /* copy the parameters into z */

   /* With this line, strings of length 1 or 2 don't go through the
      stemming process, although no mention is made of this in the
      published algorithm. Remove the line to match the published
      algorithm. */

   step1ab(z);
   if (z->k > 0) {
      step1c(z); step2(z); step3(z); step4(z); step5(z);
   }
   return z->k;
}

extern struct stemmer * create_stemmer(void)
{
    return (struct stemmer *) malloc(sizeof(struct stemmer));
    /* assume malloc succeeds */
}

extern void free_stemmer(struct stemmer * z)
{
    free(z);
}