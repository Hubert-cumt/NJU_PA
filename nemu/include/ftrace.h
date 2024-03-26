#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

void write_symbol_table_to_file(const char* filename, Elf32_Sym* symbol_table, char* string_table, int num_symbols, int string_table_size) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to open the file for writing.\n");
        return;
    }

    fprintf(file, "Symbol table:\n");
    fprintf(file, "  %-8s %-16s %-16s\n", "Index", "Name", "Value");
    for (int i = 0; i < num_symbols; i++) {
        // Check if the symbol type is FUNC (function)
        if (ELF32_ST_TYPE(symbol_table[i].st_info) == STT_FUNC) {
            fprintf(file, "  %-8d %-16s 0x%08x\n", i, &string_table[symbol_table[i].st_name], symbol_table[i].st_value);
        }
    }

    fclose(file);
}

void parse_elf_file(const char* filename) {
    // Open the ELF file
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open the ELF file.\n");
        return;
    }

    // Read the ELF header
    Elf32_Ehdr elf_header;
    size_t read_bytes = fread(&elf_header, sizeof(Elf32_Ehdr), 1, file);
    if (read_bytes != 1) {
        printf("Failed to read ELF header.\n");
        fclose(file);
        return;
    }

    // Check if it's a valid ELF file
    if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Invalid ELF file.\n");
        fclose(file);
        return;
    }

    // Read the section header table
    Elf32_Shdr* section_headers = malloc(sizeof(Elf32_Shdr) * elf_header.e_shnum);
    fseek(file, elf_header.e_shoff, SEEK_SET);
    size_t read_headers = fread(section_headers, sizeof(Elf32_Shdr), elf_header.e_shnum, file);
    if (read_headers != elf_header.e_shnum) {
        printf("Failed to read section headers.\n");
        free(section_headers);
        fclose(file);
        return;
    }

    // Find the symbol table and string table sections
    Elf32_Shdr* symbol_table_section = NULL;
    Elf32_Shdr* string_table_section = NULL;
    for (int i = 0; i < elf_header.e_shnum; i++) {
        if (section_headers[i].sh_type == SHT_SYMTAB) {
            symbol_table_section = &section_headers[i];
            break;
        } 
    }

    for (int i = 0; i < elf_header.e_shnum; i++) {
        if (section_headers[i].sh_type == SHT_STRTAB) {
            string_table_section = &section_headers[i];
            break;
        } 
    }


    // Check if the symbol table and string table sections were found
    if (symbol_table_section == NULL || string_table_section == NULL) {
        printf("Symbol table or string table section not found.\n");
        free(section_headers);
        fclose(file);
        return;
    }

    // Read the symbol table
    Elf32_Sym* symbol_table = malloc(symbol_table_section->sh_size);
    fseek(file, symbol_table_section->sh_offset, SEEK_SET);
    size_t read_symbols = fread(symbol_table, symbol_table_section->sh_size, 1, file);
    if (read_symbols != 1) {
        printf("Failed to read symbol table.\n");
        free(symbol_table);
        free(section_headers);
        fclose(file);
        return;
    }

    // Read the string table
    char* string_table = malloc(string_table_section->sh_size);
    fseek(file, string_table_section->sh_offset, SEEK_SET);
    size_t read_strings = fread(string_table, string_table_section->sh_size, 1, file);
    if (read_strings != 1) {
        printf("Failed to read string table.\n");
        free(symbol_table);
        free(string_table);
        free(section_headers);
        fclose(file);
        return;
    }

    // Write symbol table to file
    write_symbol_table_to_file("/home/hubert/ics2023/nemu/trace/symbol_table.txt", symbol_table, string_table, symbol_table_section->sh_size / sizeof(Elf32_Sym), string_table_section->sh_size);

    // Clean up
    free(symbol_table);
    free(string_table);
    free(section_headers);
    fclose(file);
}
