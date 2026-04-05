1. Implémentation de getreadcount():
    - kernel/syscall.h : ajout de #define SYS_getreadcount 22
    - kernel/syscall.c : ajout de l'extern et de l'entrée [SYS_getreadcount] dans la table des syscall
    - kernel/sysfile.c : déclaration du compteur global readcount et du spinlock readcount_lock; incrément du compteur dans sys_read() à chaque appel; Implémentation de SYS_getreadcount() qui retourne la valeur de readcount
    - kernel/file.c    : initialisation du spinlock readcount_lock dans fileinit()
    - user/user.h      : ajout de int getreadcount(void)
    - user/usys.pl     : ajout de entry("getreadcount")

2. Implémentation de getfilenum(int pid):
    - kernel/syscall.h : ajout de #define SYS_getfilenum 23
    - kernel/syscall.c : ajout de l'extern et de l'entrée [SYS_getfilenum] sys_getfilenum dans la table des syscalls
    - kernel/sysproc.c : implémentation de sys_getfilenum() qui parcourt la table des processus pour trouver le processus avec le pid donné, compte les descripteurs de fichiers valides et retourne ce nombre, ou -1 si aucun processus avec ce pid n'existe
    - user/user.h      : ajout de int getfilenum(int pid)
    - user/usys.pl     : ajout de entry("getfilenum")