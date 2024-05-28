# <p align="center">malloc</p>
> Ce projet consiste à écrire une bibliothèque de gestion de mémoire à allocation dynamique, pour que vous puissiez l'utiliser avec certains programmes déjà en cours d'utilisation sans les modifier ni les recompiler.
>
> Vous devez réécrire les fonctions libc suivantes : `malloc(3)`, `realloc(3)` et `free(3)`.

## Checklist

* [x] `void* malloc(size_t size)`: Alloue `size` octets de mémoire via `mmap`
* [x] `void* realloc(void* ptr, size_t size)`: Modifie la taille de l'allocation pointée par `ptr` à `size`
* [x] `void free(void* ptr)`: Désalloue la mémoire allouée pointée par `ptr`
* [x] `void show_alloc_mem()`: Permet de visualiser l'état des zones de mémoire allouées
* [x] `BONUS` `void show_alloc_mem_ex()`: Ajoute un memory dump à `show_alloc_mem()`
* [x] `BONUS` Défragmentation de la mémoire libérée
* [x] `BONUS` Thread-safe
* [x] `BONUS` Variables d'environnement de debug
* [x] `EXTRA` `int mallopt(int opt, int value)`: Permet de modifier le comportement de la malloc
* [x] `EXTRA` `void* calloc(size_t nmemb, size_t size)`: Alloue de la mémoire pour un tableau de `nmemb` éléments de `size` octets chacun

### Debug environnement variables

* `MEM_ALLOCATED`: Mémoire allouée pour le programme
* `MEM_IN_USE`: Mémoire allouée pour le programme et utilisée
* `MEM_FREED`: Mémoire allouée pour le programme et libérée
* `INTERN_MEM_ALLOCATED`: Mémoire allouée pour le fonctionnement de malloc
* `INTERN_MEM_FREED`: Mémoire allouée pour le fonctionnement de malloc et libérée

### Options `mallopt`

| Option | Description | Default |
|:-|:-:|-:|
| `TINY_SIZE` | Taille des blocs de mémoire fixe `TINY` | 64 |
| `SMALL_MULT` | `TINY_SIZE * SMALL_MULT` = Taille des blocs de mémoire fixe `SMALL` | 4 |
| `PROT` | Flags `prot` pour `mmap` | `PROT_READ` \| `PROT_WRITE` |
| `BZERO` | Initialise la mémoire allouée à 0 | `FALSE` |

## Install
```bash
mkdir -p ~/.local/src
mkdir -p ~/.local/lib

apt update -y
apt install -y git
apt install -y make
apt install -y gcc
```

```bash
cd ~/.local/src
git clone https://github.com/Skalyaeve/malloc.git
cd malloc && make
ln -s $PWD/libft_malloc.so ~/.local/lib/libft_malloc.so
export LD_LIBRARY_PATH=~/.local/lib:$LD_LIBRARY_PATH
```

```
LDFLAGS=-L~/.local/lib -lft_malloc
```
