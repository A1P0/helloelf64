/*
 * helloelf64
 * Real basic example in generating an ELF executable.
 * x86_64 Linux 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <elf.h>
int main() {
	Elf64_Ehdr header = {
		{ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB, EV_CURRENT, ELFOSABI_SYSV, 0, EI_PAD, EI_NIDENT},
		ET_EXEC,		/* This file is executable */
		EM_X86_64,		/* This is for x86_64 */
		EV_CURRENT,		/* Current version, just do it */
		0x40000 +		/* Virtual address entry point of the program */
		sizeof(Elf64_Ehdr)+
		sizeof(Elf64_Phdr),
		sizeof(Elf64_Ehdr), 	/* program header tables files offset in bytes,
						in this case it is right after this header */
		0, 			/* File offset of the section header table */
		0,			/* Processor-specific flags--there aren't any. */
		sizeof(Elf64_Ehdr),	/* Size of this header */
		sizeof(Elf64_Phdr),	/* Size of one entry in the program header table */
		1,			/* Number of entries in the program header table, 2 in our case */
		0,			/* Size of one entry in the section header table */
		0,			/* Number of entries in the section header table */
		SHN_UNDEF		/* Section header table index (the section) that contains
						the section name string table */
	};
	Elf64_Phdr text_segment_header = {
			PT_LOAD,		/* p_type: What type of segment, what to do with it. This will be loaded into memory */
			PF_X ^ PF_R,		/* p_flags: flags for the segment, permissions basically. Making executable and readable */
			0,			/* p_offset: this defines where in the file the first byte of actual segment data resides */
			0x40000,		/* p_vaddr: virtual address where the first byte of the segment will be loaded in memory */
			0,			/* p_paddr: Irrelevant. Only for systems where physical addressing is relevant, ignore this */
			49,			/* p_filesz: number of bytes of data in the segment in the file */
			49,			/* p_memsz: number of bytes the segment will take up in memory */
			0x1000			/* p_align: If the segment has alignment requirements that goes here, if not leave 0. */
	};
	char text_segment[49] = {
		0xb8, 0x01, 0x00, 0x00, 0x00,	/* mov rax, 1 		; select write() system call */
		0xbf, 0x01, 0x00, 0x00, 0x00,   /* mov rdi, 1 		; select stdout as file descriptor */
		0xbe, 0x9A, 0x00, 0x04, 0x00,	/* mov rsi, 0x4009A 	; move pointer to our string to rsi */
		0xba, 0x0F, 0x00, 0x00, 0x00,	/* mov rdx, 0x0F	; move the length of our string to rdx */
		0x0f, 0x05,			/* syscall 		; actually call write() */
		0xb8, 0x3c, 0x00, 0x00, 0x00,	/* mov rax, 60		; select exit() system call */
		0xbf, 0x01, 0x00, 0x00, 0x00,	/* mov rdi, 0 		; exit status 0 for success */
		0x0f, 0x05,			/* syscall		; actually call exit() */
		'H', 'e', 'l', 'l', 'o', ',',
		' ', 'E', 'l', 'f', '6', '4', '!',	/* our string at the end waiting to be referenced */
		'\n'				
	};
	/*
	 * Open file to write program contents and produce executable
	*/
	FILE *file;
	size_t check;
	file = fopen("helloelf64", "wb");
	if (file == NULL) {
		perror("Could not open file \"helloelf64\"");
		return EXIT_FAILURE;
	}
	fwrite(&header, sizeof(Elf64_Ehdr), 1, file);
	fwrite(&text_segment_header, sizeof(Elf64_Phdr), 1, file);
	check = fwrite(&text_segment, sizeof(char), 49, file);
	if (check < 1) {
		perror("Could not write to file \"helloelf64\"");
		return EXIT_FAILURE;
	}
	fclose(file);
	if(chmod("helloelf64", S_IRWXU) != 0) {
		perror("Could not make helloelf64 executable");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
