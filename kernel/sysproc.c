#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgpte(void)
{
  uint64 va;
  struct proc *p;  

  p = myproc();
  argaddr(0, &va);
  pte_t *pte = pgpte(p->pagetable, va);
  if(pte != 0) {
      return (uint64) *pte;
  }
  return 0;
}
#endif

#ifdef LAB_PGTBL
int
sys_kpgtbl(void)
{
  struct proc *p;  

  p = myproc();
  vmprint(p->pagetable);
  return 0;
}
#endif


uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// System call: pgaccess(void *base, int len, void *mask)
// Reports which pages have been accessed by checking PTE_A bit.
uint64
sys_pgaccess(void)
{
  uint64 base;           // Starting virtual address
  int len;               // Number of pages to check
  uint64 user_mask_addr; // User buffer address for result bitmask
  struct proc *p = myproc();
  
  // Parse arguments from user space
  argaddr(0, &base);
  argint(1, &len);
  argaddr(2, &user_mask_addr);
  
  // Validate arguments - maximum 64 pages (64-bit bitmask limit)
  if (len < 0 || len > 64) {
    return -1;
  }
  
  // Temporary kernel buffer to store bitmask
  // Each bit represents one page: bit i -> page i was accessed
  uint64 bitmask = 0;
  
  // Check each page for access bit
  for (int i = 0; i < len; i++) {
    // Calculate virtual address of current page
    uint64 va = base + i * PGSIZE;
    
    // Walk page table to find PTE for this virtual address
    // walk() returns pointer to PTE, or 0 if page not mapped
    pte_t *pte = walk(p->pagetable, va, 0);
    
    if (pte == 0) {
      // Page not mapped - skip it
      continue;
    }
    
    // Check if PTE is valid
    if (*pte & PTE_V) {
      // Check if access bit (PTE_A) is set by hardware
      if (*pte & PTE_A) {
        // Page was accessed - set corresponding bit in bitmask
        bitmask |= (1L << i);
        
        // CRITICAL: Clear the access bit for next check
        // Without this, the bit stays set forever!
        *pte &= ~PTE_A;
      }
    }
  }
  
  // Copy bitmask from kernel space to user space
  if (copyout(p->pagetable, user_mask_addr, (char *)&bitmask, sizeof(bitmask)) < 0) {
    return -1;
  }
  
  return 0;  // Success
}