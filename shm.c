#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {
//you write this
//1. Look through the shm_table to see if the id we are opening already exists.
int i;
int id_exists;

struct proc * curproc = myproc();

acquire(&(shm_table.lock));

for (i = 0; i < 64; i++) {
    if (shm_table.shm_pages[i].id == id) {
        id_exists = 1;
        break;
    }
}
//case 1: already exists. find physical address of page in table, map it to available page in v address space
if(id_exists == 1) {
    int p_addr = V2P(shm_table.shm_pages[i].frame); //V2P the page address
    mappages(curproc->pgdir, (void*)PGROUNDUP(curproc->sz), PGSIZE, p_addr, PTE_W|PTE_U);
    
    shm_table.shm_pages[i].refcnt++; // increment reference counter of page in table
    *pointer = (char*)PGROUNDUP(curproc->sz);
    curproc->sz = PGROUNDUP(curproc->sz) + PGSIZE;
}
//case 2: first time calling shm_open.
//a) find empty entry in shm_table, set id to id passed in
//b) kmalloc a page, store address in frame
//c) set reference count to 1
//d) repeat page map from case 1
else {
    for (i = 0; i < 64; i++) {
        if (shm_table.shm_pages[i].id == 0 && shm_table.shm_pages[i].frame == 0 && shm_table.shm_pages[i].refcnt == 0) {
            shm_table.shm_pages[i].id = id; // set id
            //allocate for frame
            shm_table.shm_pages[i].frame = kalloc();
            memset(shm_table.shm_pages[i].frame, 0, PGSIZE);
            //reference count = 1
            shm_table.shm_pages[i].refcnt = 1;

            int p_addr = V2P(shm_table.shm_pages[i].frame); //V2P the page address
            mappages(curproc->pgdir, (void*)PGROUNDUP(curproc->sz), PGSIZE, p_addr, PTE_W|PTE_U);
            *pointer = (char*)PGROUNDUP(curproc->sz);
            curproc->sz = PGROUNDUP(curproc->sz) + PGSIZE;
            break;
        }
    }
}
release(&(shm_table.lock));

return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
//you write this too!




return 0; //added to remove compiler warning -- you should decide what to return
}
