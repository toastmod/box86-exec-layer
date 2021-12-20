
/* exec LD_PRELOAD layer */
/* credits:
https://code.woboq.org/userspace/glibc/posix/execl.c.html
https://code.woboq.org/userspace/glibc/posix/execle.c.html vbn
https://stackoverflow.com/questions/37911702/overriding-execve-with-ld-preload-only-works-sometimes
https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header
 */
#define _GNU_SOURCE
#include<unistd.h>
#include<dlfcn.h>
#include<stdio.h>
#include<stdarg.h>
#include<stddef.h>
#include<stdbool.h>
#include<limits.h>
#include<errno.h>
#include<elf.h>
#include<string.h>
#include "boxec.h"

#ifdef BOX64SYS
#define ELFW(TYPE) Elf64_ ## TYPE
#define BOXTARGET 0x3E
const char* INTERP_NAME = "box64";
#else
#define ELFW(TYPE) Elf32_ ## TYPE
#define BOXTARGET 0x03
const char* INTERP_NAME = "box86";
#endif

// // This macro is based on the actual execl* source code that packs variadic args into an array.
// #define MAKE_ARGV(PATHFILE,ARGVNAME){\
//     ptrdiff_t i;\
//     char* ARGVNAME[argc + 2];\
//     ARGVNAME[0] = PATHFILE;\
//     va_start (ap, arg);\
//     ARGVNAME[1] = (char *) arg;\
//     for (i = 2; i <= argc; i++) ARGVNAME[i] = va_arg (ap, char *);\
//     va_end (ap);\
// }

// #define MAKE_ARGV_ARM(ARGVNAME){\
//     ptrdiff_t i;\
//     char* ARGVNAME[argc + 1];\
//     va_start (ap, arg);\
//     ARGVNAME[0] = (char *) arg;\
//     for (i = 1; i <= argc; i++) ARGVNAME[i] = va_arg (ap, char *);\
//     va_end (ap);\
// }


/* Determine the architecture of the executable */
bool is_arm(const char* pathfile){
    bool it_is = true; 
    ELFW(Ehdr) header; 
    FILE* f = fopen(pathfile, "rb");
    if(f){
        fread(&header, sizeof(header), 1, f);
        if(memcmp(header.e_ident, ELFMAG, SELFMAG) == 0){
            // valid ELF file, read arch
            if(header.e_machine == BOXTARGET) {
                // invoke the interpreter
                printf("[BOXEC] Non-ARM ELF detected!\n");
                it_is = false;
                // otherwise, try and fail to load whatever it is, like normal.
            }else{
                //printf("[BOXEC] ARM ELF passing through\n");
            }

        }else{
            // it's not an ELF, don't invoke the interpreter.
            //printf("[BOXEC] This is a Non-ELF file\n");
            it_is = true;
        }
    } 
    //printf("[BOXEC] Closing file reader...\n");
    fclose(f); 
    //printf("[BOXEC] Returning...\n");
    return it_is;    
}

size_t count_args(char* args[]) {
    size_t i = 0;
    char* cursor = args[i];
    //printf("[BOXEC] Recounting args...\n");
    while(cursor != (char*)NULL) {
        i++;
        cursor = args[i];
    }
    i++;
    return i;
}

/* SHIMMY YEAH SHIMMY YEAYuHHHH */
/* note: new must always be 1 address larger than old */
void shimmy(size_t new_len, char* (*old)[], char* (*new)[]) {
    //printf("[BOXEC] Shimmying args...\n");
    for(size_t i=2; i<new_len; i++){
        (*new)[i] = (*old)[i-1];
    }
    return;
}

/* EXECV_ FUNCTIONS */
/* EXECV */
// int execv(const char* pathname, char* const argv[]) {
//     ////printf("[BOXEC] execv hook\n");
//     old_execv = dlsym(RTLD_NEXT, "execv");

//     if(is_arm(pathname)){
//         return old_execv(pathname, argv);
//     }else{
//         size_t len = count_args(argv)+1;
//         char* new_argv[len];
//         shimmy(len,&argv,&new_argv);
//         new_argv[0] = pathname;
//         return old_execv(INTERP_NAME,new_argv);
//     }
// }

/* EXECVE */
int execve(const char* pathname, char* const argv[], char* const envp[]) {
    ////printf("[BOXEC] execve hook\n");
    if(is_arm(pathname)){
        old_execve = dlsym(RTLD_NEXT, "execve");
        return old_execve(pathname, argv, envp);
    }else{
        old_execvpe = dlsym(RTLD_NEXT, "execvpe");
        size_t len = count_args(argv)+2;
        char* new_argv[len];
        shimmy(len,&argv,&new_argv);
        new_argv[1] = pathname;
        printf("[BOXEC] >>> %s%s%s ... \n",INTERP_NAME,argv[0],argv[1]);
        return old_execvpe(INTERP_NAME, new_argv, envp);
    }
}

/* EXECVP */
// int execvp(const char* file, char* const argv[]) {
//     //printf("[BOXEC] execvp hook\n");
//     old_execvp = dlsym(RTLD_NEXT, "execvp");
    
//     if(is_arm(file)){
//         return old_execvp(file, argv);
//     }else{
//         size_t len = count_args(argv)+1;
//         char* new_argv[len];
//         shimmy(len,&argv,&new_argv);
//         new_argv[0] = file;
//         return old_execvp(INTERP_NAME,new_argv);
//     }
// }

// /* EXECVPE */
// int execvpe(const char* file, char* const argv[], char* const envp[]) {
//     //printf("[BOXEC] execvpe hook\n");
//     old_execve = dlsym(RTLD_NEXT, "execvpe");
//     if(is_arm(file)){
//         return old_execvpe(file, argv, envp);
//     }else{
//         size_t len = count_args(argv)+1;
//         char* new_argv[len];
//         shimmy(len,&argv,&new_argv);
//         new_argv[0] = file;
//         return old_execvpe(INTERP_NAME,new_argv,envp);
//     }
// }

/* VARIADICS AHEAD BEWARE */

/* EXECL */

// int execl(const char* pathname, const char* arg, ...) {
//     //printf("[BOXEC] execl hook\n");
//     // old_execl = dlsym(RTLD_NEXT, "execl");
//     // return old_execl(pathname, arg, NULL);

//     /* count argc */
//     ptrdiff_t argc;
//     va_list ap;
//     va_start (ap, arg);
//     for (argc = 1; va_arg (ap, const char *); argc++){
//         if (argc == INT_MAX)
//         {
//             va_end (ap);
//             errno = E2BIG;
//             return -1;
//         }
//     }
//     va_end (ap);

//     /* define sized array */
//     ptrdiff_t i;
//     char* argv[argc + 1];

//     /* arrange args */
//     va_start (ap, arg);
//     argv[0] = (char *) arg;
//     for (i = 1; i <= argc; i++) argv[i] = va_arg (ap, char *);
//     va_end (ap);
    
//     /* pass to execv_ */
//     return execv(pathname, argv);

// }

// /* EXECLP */
// int execlp(const char* file, const char* arg, ...) {
//     //printf("[BOXEC] execlp hook\n");
//     // old_execlp = dlsym(RTLD_NEXT, "execlp");
//     // return old_execlp(file, arg, NULL);

//     /* count argc */
//     ptrdiff_t argc;
//     va_list ap;
//     va_start (ap, arg);
//     for (argc = 1; va_arg (ap, const char *); argc++){
//         if (argc == INT_MAX)
//         {
//             va_end (ap);
//             errno = E2BIG;
//             return -1;
//         }
//     }
//     va_end (ap);

//     /* define sized array */
//     ptrdiff_t i;
//     char* argv[argc + 1];

//     /* arrange args */
//     va_start (ap, arg);
//     argv[0] = (char *) arg;
//     for (i = 1; i <= argc; i++) argv[i] = va_arg (ap, char *);
//     va_end (ap);
    
//     /* pass to execv_ */
//     return execvp(file, argv);

// }

// int execle(const char* pathname, const char* arg, ... ) {
//     //printf("[BOXEC] execle hook\n");
//     // old_execle = dlsym(RTLD_NEXT, "execle");
//     // return old_execle(pathname, arg, NULL, envp);

//     /* count argc */
//     ptrdiff_t argc;
//     va_list ap;
//     va_start (ap, arg);
//     for (argc = 1; va_arg (ap, const char *); argc++){
//         if (argc == INT_MAX)
//         {
//             va_end (ap);
//             errno = E2BIG;
//             return -1;
//         }
//     }
//     va_end (ap);

//     /* define sized array */
//     ptrdiff_t i;
//     char** envp;
//     char* argv[argc + 1];

//     /* arrange args */
//     va_start (ap, arg);
//     argv[0] = (char *) arg;
//     for (i = 1; i <= argc; i++) argv[i] = va_arg (ap, char *);
//     envp = va_arg(ap, char**);
//     va_end (ap);
    
//     /* pass to execv_ */
//     return execvpe(pathname, argv, envp);

// }
