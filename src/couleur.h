#ifndef COULEUR
#define COULEUR

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

inline void red()   { printf("%s", KRED); }
inline void yellow(){ printf("%s", KYEL); }
inline void green() { printf("%s", KGRN); }
inline void black() { printf("%s", KNRM); }
inline void magenta() { printf("%s", KMAG); }
inline void red  (const char* txt){ printf("%s%s%s", KRED, txt, KNRM); }
inline void green(const char* txt){ printf("%s%s%s", KGRN, txt, KNRM); }

#endif