1. Implémentation de getreadcount():
    - kernel/syscall.h : ajout de #define SYS_getreadcount 22
    - kernel/syscall.c : ajout de l'extern et de l'entrée [SYS_getreadcount] dans la table des syscall
    - kernel/sysfile.c : déclaration du compteur global readcount et du spinlock readcount_lock; incrément du compteur dans sys_read() à chaque appel; Implémentation de SYS_getreadcount() qui retourne la valeur de readcount
    - kernel/file.c    : initialisation du spinlock readcount_lock dans fileinit()
    - user/user.h      : ajout de int getreadcount(void)
    - user/usys.pl     : ajout de entry("getreadcount")

2. Implémentation de getfilenum(int pid):
