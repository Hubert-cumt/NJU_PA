#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
# define EXPECT_TYPE EM_386
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV
#else
# error Unsupported ISA
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  // create a temp_buf to store the ELF Header
  // char temp_buf[TEMP_SIZE];
  // unsigned char* temp_buf_p = temp_buf

  // calculate the offset of the file
  int fd = fs_open(filename, 0, 0);
  
  Log("fd: %d", fd);
  // read the ELF file from ramdisk 
  Elf_Ehdr ehdr;
  // ramdisk_read(&ehdr, pos, sizeof(Elf_Ehdr));
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));

  // check the magic number 
  if(ehdr.e_ident[EI_MAG0] != 0x7F ||
     ehdr.e_ident[EI_MAG1] != 'E' ||
     ehdr.e_ident[EI_MAG2] != 'L' ||
     ehdr.e_ident[EI_MAG3] != 'F') {
      Log("This is not ELF file !");
      assert(0);
  }

  // check the ELF-ISA
  if(ehdr.e_machine != EXPECT_TYPE) {
    Log("THE ELF-ISA is not matched !");
    assert(0);
  }

  // get the every segment which need LOAD
  Elf_Phdr phdr;
  // fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  for(int i = 0; i < ehdr.e_phnum; i++) {
    fs_lseek(fd, ehdr.e_phoff + i * sizeof(Elf_Phdr), SEEK_SET);
    // ramdisk_read(&phdr, ehdr.e_phoff + i * ehdr.e_phentsize, sizeof(Elf_Phdr));
    fs_read(fd, &phdr, sizeof(Elf_Phdr));
    

    // check whether the Type is PT_LOAD
    if(phdr.p_type == PT_LOAD) {
      size_t Offset = phdr.p_offset;
      size_t Filesize = phdr.p_filesz;
      Elf32_Addr Virtaddr = phdr.p_vaddr;
      size_t Memsize = phdr.p_memsz;

      Log("Vitraddr: %x", Virtaddr);
      Log("Filesize: %d", Filesize);
      Log("Memsize: %d", Memsize);
      Log("AreaTail: %x", Virtaddr + Memsize);

      char* buffer = (char *)malloc(Filesize);
      // ramdisk_read(buffer, Offset, Filesize);
      fs_lseek(fd, Offset, SEEK_SET);
      fs_read(fd, buffer, Filesize);
      
      // use filesize bc ONLY this length. TIP: memcpy NOT check you length
      memcpy((void *)(uintptr_t)Virtaddr, buffer, Filesize);
      
      if(Memsize > Filesize) {
        char* temp = (char*)malloc(Memsize - Filesize);
        memset(temp, 0, sizeof(temp));
        
        memcpy((void *)(uintptr_t)Virtaddr + Filesize, temp, Memsize - Filesize);
      }
    }
  }
  
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

