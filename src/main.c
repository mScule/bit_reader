#include <stdio.h>  // printf(), fopen(), fseek(), fread(), ftell(), fclose(), FILE
#include <stdlib.h> // malloc(), free()
#include <stdint.h> // uint32_t, uint16_t, uint8_t

#include "lib/shared_macros.h"
#include "lib/str.h"

// Program

#define OUTPUT_NAME_DEFAULT "a"

#define ARG_ENDIANNESS      "-e"
#define ARG_ENDIANNESS_LONG "--endianness"

#define ARG_OUTPUT_NAME      "-o"
#define ARG_OUTPUT_NAME_LONG "--output"

enum {
    ENDIANNESS_LITTLE,
    ENDIANNESS_BIG
};

// Syntax

#define ONE  '1'
#define ZERO '0'

// File reading

// Buffer that contains the input bytes as an array.
uint8_t *input_buffer, endianness = ENDIANNESS_LITTLE;

char *input_file_path;

uint32_t input_len, input_index = 0;
uint8_t cur_input_char;

// Current Byte

uint8_t
    cur_byte = 0,
    cur_bit  = 8,
    bit_arr[8];

// Output

/*
   Used by the writing function to write byte to correct place
   into the output buffer.

   Also used by the export function to check the location of last
   written byte.
*/
uint32_t cur_mem_loc = 0;

// Buffer where the output is first written in.
uint8_t *output_buffer;

char *output_file_name = OUTPUT_NAME_DEFAULT;

FILE *output_export;

// Functions

void write(uint8_t byte) {
    output_buffer[cur_mem_loc] = byte;
    cur_mem_loc++;
}

void fill_byte(uint8_t bit) {
    cur_bit--;

    bit_arr[cur_bit] = bit;

    if (cur_bit == 0) {
        if(endianness == ENDIANNESS_LITTLE) {
            cur_byte += bit_arr[0] << 0;
            cur_byte += bit_arr[1] << 1;
            cur_byte += bit_arr[2] << 2;
            cur_byte += bit_arr[3] << 3;
            cur_byte += bit_arr[4] << 4;
            cur_byte += bit_arr[5] << 5;
            cur_byte += bit_arr[6] << 6;
            cur_byte += bit_arr[7] << 7;

        } else if (endianness == ENDIANNESS_BIG) {
            cur_byte += bit_arr[7] << 0;
            cur_byte += bit_arr[6] << 1;
            cur_byte += bit_arr[5] << 2;
            cur_byte += bit_arr[4] << 3;
            cur_byte += bit_arr[3] << 4;
            cur_byte += bit_arr[2] << 5;
            cur_byte += bit_arr[1] << 6;
            cur_byte += bit_arr[0] << 7;
        }

        write(cur_byte);

        cur_byte = 0;
        cur_bit  = 8;
    }
}

void get_next_char() {
    if (input_index < input_len)
        cur_input_char = input_buffer[input_index++];
    else
        cur_input_char = '\0';
}

int is_one_or_zero() {
    if (cur_input_char == ONE || cur_input_char == ZERO)
        return 1;
    return 0;
}

void export() {
    for (uint32_t i = 0; i < cur_mem_loc; i++) {
        uint8_t byte = output_buffer[i];
        fwrite(&byte, sizeof(uint8_t), 1, output_export);
    }
}

void error(char *err_msg) {
    printf("%s %s error: %s", PROJECT_NAME, PROJECT_VERSION, err_msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 0;

    // Reading through arguments
    else {
        for (uint16_t i = 0; i < argc; i++) {
            if (
                str_equals(argv[i], ARG_ENDIANNESS) ||
                str_equals(argv[i], ARG_ENDIANNESS_LONG)) {

                if (argc - 1 < i + 1)
                    error("no given value for endianness");
                else {
                    i++;

                    if (str_equals(argv[i],"little"))
                        endianness = ENDIANNESS_LITTLE;
                    else if (str_equals(argv[i], "big"))
                        endianness = ENDIANNESS_BIG;
                    else
                        error("unsupported endianness type");
                }
            } else if (
                str_equals(argv[i], ARG_OUTPUT_NAME) ||
                str_equals(argv[i], ARG_OUTPUT_NAME_LONG)) {

                if (argc - 1 < i + 1)
                    error("no given value for output file name");
                else {
                    i++;
                    output_file_name = argv[i];
                }
            } else {
                input_file_path = argv[i];
            }
        }
    }

    printf("%s %s \nFile: %s\n", PROJECT_NAME, PROJECT_VERSION, input_file_path);

    // Reading input file.
    FILE *input_file = fopen(input_file_path, "rb");

    // Counting file length.
    fseek(input_file, 0x0, SEEK_END);
    input_len = ftell(input_file);
    fseek(input_file, 0x0, SEEK_SET);

    // Creating inputbuffer & output buffer.
    input_buffer  = malloc(sizeof(uint8_t) * input_len + 1);
    output_buffer = malloc(sizeof(uint8_t) * UINT16_MAX * 4);

    fread(input_buffer, sizeof(char) * input_len, 1, input_file);
    fclose(input_file);

    input_buffer[input_len] = '\0';

    // Printing info.
    printf("Program size: %d bytes\n", input_len);

    // Endianness
    char *en_str;

    switch (endianness) {
        case ENDIANNESS_LITTLE: en_str = "Little"; break;
        case ENDIANNESS_BIG:    en_str = "Big";    break;
    }
    printf("Endianness: %s\n", en_str);

    printf("Exporting to: %s\n", output_file_name);

    // Reading file.

    get_next_char();

    while (cur_input_char != '\0') {

        if (is_one_or_zero()) {
            if (cur_input_char == ONE)
                fill_byte(1);
            else if (cur_input_char == ZERO)
                fill_byte(0);
        }

        get_next_char();
    }

    output_export = fopen(output_file_name, "wb+");
    export();
    fclose(output_export);

    free(output_file_name);
    free(input_buffer);
    free(output_buffer);
}
