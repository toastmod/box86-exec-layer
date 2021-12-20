/* EXECVPE */
typedef ssize_t (*execvpe_func_t)(const char* file, char *const argv[] , char* const envp[]);
static execvpe_func_t old_execvpe = NULL;

/* EXECVP */
typedef ssize_t (*execvp_func_t)(const char* file, char* const argv[]);
static execvp_func_t old_execvp = NULL;

/* EXECVE */
typedef ssize_t (*execve_func_t)(const char* pathname, char* const argv[], char* const envp[]);
static execve_func_t old_execve = NULL;

/* EXECV */
typedef ssize_t (*execv_func_t)(const char* pathname, char* const argv[]);
static execv_func_t old_execv = NULL;

/* EXECL_ FUNCS */

/* EXECLE */
typedef ssize_t (*execle_func_t)(const char* file, char *const argv[] , char* const envp[]);
static execle_func_t old_execle = NULL;

/* EXECLP */
typedef ssize_t (*execlp_func_t)(const char* file, char* const argv[]);
static execlp_func_t old_execlp = NULL;

/* EXECV */
typedef ssize_t (*execl_func_t)(const char* pathname, char* const argv[]);
static execl_func_t old_execl = NULL;


