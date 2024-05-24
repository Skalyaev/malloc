# <p align="center">malloc</p>
> Ce projet consiste à écrire une bibliothèque de gestion de mémoire à allocation dynamique, pour que vous puissiez l'utiliser avec certains programmes déjà en cours d'utilisation sans les modifier ni les recompiler.
>
> Vous devez réécrire les fonctions libc suivantes : `malloc(3)`, `realloc(3)` et `free(3)`.

## Checklist

* [ ] `void* malloc(size_t size)`: Alloue `size` octets de mémoire et renvoie un pointeur vers la zone mémoire allouée
* [ ] `void* realloc(void *ptr, size_t size)`: Modifie la taille de l'allocation pointée par `ptr` à `size`, et renvoie `ptr`
* [ ] `void* free(void *ptr)`: Désalloue la mémoire allouée pointée par `ptr`
* [ ] `void show_alloc_mem_ex()`: Permet de visualiser l'état des zones de mémoire allouées
* [ ] Debug environment variables
* [ ] Défragmentation de la mémoire libérée
