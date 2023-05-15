#ifndef VOXELIZE__script
#define VOXELIZE__script

/* Include */
// standard
#include "../base/basic.h"

// print
#include <stdio.h>

/* Define */
#define SCRIPT__register_count 256
#define SCRIPT__unused_register 0

/* Type */
// registers
typedef BASIC__address SCRIPT__register;

// instructions
typedef u8 SCRIPT__instruction_ID;
typedef u8 SCRIPT__register_ID;
typedef u8 SCRIPT__operation_ID;
typedef u8 SCRIPT__flag_ID;
typedef u8 SCRIPT__byte_size;
typedef u16 SCRIPT__value_ID;
typedef u64 SCRIPT__max_integer_width_type;

// statistics
typedef u64 SCRIPT__counter;
typedef u64 SCRIPT__instruction_length;
typedef u64 SCRIPT__offset;

// deprecating soon
typedef BASIC__address SCRIPT__register;
typedef SCRIPT__register SCRIPT__value;

/* Error */
// error type
typedef enum SCRIPT__et {
    SCRIPT__et__no_error,
    SCRIPT__et__invalid_value_ID,
    SCRIPT__et__invalid_byte_size, // fatal
    SCRIPT__et__invalid_operation, // fatal
    SCRIPT__et__divide_by_zero,
    SCRIPT__et__modulo_by_zero,
    SCRIPT__et__buffer_allocation_failure,
    SCRIPT__et__invalid_file_address,
    SCRIPT__et__unable_to_fully_read_file_into_buffer,
    SCRIPT__et__unable_to_fully_write_buffer_into_file,
    SCRIPT__et__invalid_instruction_ID, // fatal
    SCRIPT__et__LENGTH,
} SCRIPT__et;

/* Registers */
typedef struct SCRIPT__registers {
    SCRIPT__register p_registers[SCRIPT__register_count];
} SCRIPT__registers;

// register type
typedef enum SCRIPT__rt {
    SCRIPT__rt__instruction_address,
    SCRIPT__rt__error,
    SCRIPT__rt__flags_0,
    SCRIPT__rt__flags_1,
    SCRIPT__rt__flags_2,
    SCRIPT__rt__flags_3,
    SCRIPT__rt__output_buffer_address,
    SCRIPT__rt__output_buffer_length,
    SCRIPT__rt__LENGTH,
} SCRIPT__rt;

BASIC__bt SCRIPT__get__bit(u8* address, SCRIPT__max_integer_width_type bit_offset) {
    return (BASIC__bt)((address[bit_offset / 8] & (1 << (bit_offset % 8))) != BASIC__bt__false);
}

SCRIPT__register SCRIPT__read__register(SCRIPT__registers* registers, SCRIPT__register_ID register_ID) {
    return (*registers).p_registers[register_ID];
}

u8 SCRIPT__read__register_as_u8(SCRIPT__registers* registers, SCRIPT__register_ID register_ID) {
    return (u8)(SCRIPT__max_integer_width_type)(*registers).p_registers[register_ID];
}

u16 SCRIPT__read__register_as_u16(SCRIPT__registers* registers, SCRIPT__register_ID register_ID) {
    return (u16)(SCRIPT__max_integer_width_type)(*registers).p_registers[register_ID];
}

u32 SCRIPT__read__register_as_u32(SCRIPT__registers* registers, SCRIPT__register_ID register_ID) {
    return (u32)(SCRIPT__max_integer_width_type)(*registers).p_registers[register_ID];
}

u64 SCRIPT__read__register_as_u64(SCRIPT__registers* registers, SCRIPT__register_ID register_ID) {
    return (u64)(SCRIPT__max_integer_width_type)(*registers).p_registers[register_ID];
}

void SCRIPT__set__bit(u8* address, SCRIPT__max_integer_width_type bit_offset, BASIC__bt value) {
    u8 mask;

    // setup mask
    mask = ~(1 << (bit_offset & 7));
    
    // apply mask to temp
    address[bit_offset / 8] &= mask;

    // set bit if appropriate
    if (value != BASIC__bt__false) {
        address[bit_offset / 8] += (~mask);
    }

    return;
}

void SCRIPT__write__register(SCRIPT__registers* registers, SCRIPT__register_ID register_ID, SCRIPT__register value) {
    (*registers).p_registers[register_ID] = value;

    return;
}

void SCRIPT__write__register_with_address(SCRIPT__registers* registers, SCRIPT__register_ID register_ID, BASIC__address value) {
    (*registers).p_registers[register_ID] = (SCRIPT__register)value;

    return;
}

void SCRIPT__write__register_with_u8(SCRIPT__registers* registers, SCRIPT__register_ID register_ID, u8 value) {
    (*registers).p_registers[register_ID] = (SCRIPT__register)(SCRIPT__max_integer_width_type)value;

    return;
}

void SCRIPT__write__register_with_u16(SCRIPT__registers* registers, SCRIPT__register_ID register_ID, u16 value) {
    (*registers).p_registers[register_ID] = (SCRIPT__register)(SCRIPT__max_integer_width_type)value;

    return;
}

void SCRIPT__write__register_with_u32(SCRIPT__registers* registers, SCRIPT__register_ID register_ID, u32 value) {
    (*registers).p_registers[register_ID] = (SCRIPT__register)(SCRIPT__max_integer_width_type)value;

    return;
}

void SCRIPT__write__register_with_u64(SCRIPT__registers* registers, SCRIPT__register_ID register_ID, u64 value) {
    (*registers).p_registers[register_ID] = (SCRIPT__register)(SCRIPT__max_integer_width_type)value;

    return;
}

void SCRIPT__print__register_as_number(SCRIPT__register register_value) {
    printf("%lu", (SCRIPT__max_integer_width_type)register_value);
    fflush(stdout);

    return;
}

void SCRIPT__print__all_registers(SCRIPT__registers registers) {
    SCRIPT__counter columns = 8;

    for (SCRIPT__counter i = 0; i < SCRIPT__register_count; i += columns) {
        printf("[ ");
        
        for (SCRIPT__counter j = 0; j < columns; j++) {
            SCRIPT__print__register_as_number(registers.p_registers[i + j]);
            printf(" ");
        }

        printf("]\n");
    }

    return;
}

/* Instructions */
// instruction type
typedef enum SCRIPT__it {
    SCRIPT__it__quit,
    SCRIPT__it__get_value,
    SCRIPT__it__write_register,
    SCRIPT__it__allocate,
    SCRIPT__it__deallocate,
    SCRIPT__it__buffer_to_register,
    SCRIPT__it__register_to_register,
    SCRIPT__it__register_to_buffer,
    SCRIPT__it__operate,
    SCRIPT__it__read_file_into_buffer,
    SCRIPT__it__write_buffer_into_file,
    SCRIPT__it__call_new_byte_machine,
    SCRIPT__it__debug__print_register,
    SCRIPT__it__debug__print_character,
    SCRIPT__it__debug__print_buffer,
    SCRIPT__it__debug__print_all_registers,
    SCRIPT__it__LENGTH,
} SCRIPT__it;

// instruction length type
typedef enum SCRIPT__ilt {
    SCRIPT__ilt__quit = sizeof(SCRIPT__instruction_ID),
    SCRIPT__ilt__get_value = sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__value_ID) + sizeof(SCRIPT__register_ID),
    SCRIPT__ilt__write_register = sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register) + sizeof(SCRIPT__register_ID),
    SCRIPT__ilt__allocate = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 3),
    SCRIPT__ilt__deallocate = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 2),
    SCRIPT__ilt__buffer_to_register = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 2) + sizeof(SCRIPT__byte_size),
    SCRIPT__ilt__register_to_register = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 2),
    SCRIPT__ilt__register_to_buffer = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 2) + sizeof(SCRIPT__byte_size),
    SCRIPT__ilt__operate = sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID) + sizeof(SCRIPT__operation_ID) + (sizeof(SCRIPT__register_ID) * 3),
    SCRIPT__ilt__read_file_into_buffer = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 4),
    SCRIPT__ilt__write_buffer_into_file = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 4),
    SCRIPT__ilt__call_new_byte_machine = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 9),
    SCRIPT__ilt__debug__print_register = sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID),
    SCRIPT__ilt__debug__print_character = sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID),
    SCRIPT__ilt__debug__print_buffer = sizeof(SCRIPT__instruction_ID) + (sizeof(SCRIPT__register_ID) * 2),
    SCRIPT__ilt__debug__print_all_registers = sizeof(SCRIPT__instruction_ID),
} SCRIPT__ilt;

/* Operations */
// operation type
typedef enum SCRIPT__ot {
    // integer
    SCRIPT__ot__integer__add,
    SCRIPT__ot__integer__subtract,
    SCRIPT__ot__integer__multiply,
    SCRIPT__ot__integer__divide,
    SCRIPT__ot__integer__modulo,

    // binary
    SCRIPT__ot__binary__left_shift,
    SCRIPT__ot__binary__right_shift,
    SCRIPT__ot__binary__not,
    SCRIPT__ot__binary__or,
    SCRIPT__ot__binary__and,
    SCRIPT__ot__binary__xor,
    SCRIPT__ot__binary__wipe_bits,

    // flag
    SCRIPT__ot__flag__and,
    SCRIPT__ot__flag__or,
    SCRIPT__ot__flag__not,
    SCRIPT__ot__flag__xor,
    SCRIPT__ot__flag__write_flag,
    SCRIPT__ot__flag__move_flag,
    SCRIPT__ot__flag__get_flag,

    // integer comparison
    SCRIPT__ot__integer__less_than,
    SCRIPT__ot__integer__not_equal_to,
    SCRIPT__ot__integer__equal_to,
    SCRIPT__ot__integer__greater_than,

    // move
    SCRIPT__ot__static__register_to_register,
    SCRIPT__ot__dynamic__register_to_register,

    // operation length
    SCRIPT__ot__LENGTH,
} SCRIPT__ot;

/* Values */
// value type
typedef enum SCRIPT__vt {
    SCRIPT__vt__zero,
    SCRIPT__vt__one,
    SCRIPT__vt__ilt__quit,
    SCRIPT__vt__ilt__get_value,
    SCRIPT__vt__ilt__write_register,
    SCRIPT__vt__ilt__allocate,
    SCRIPT__vt__ilt__deallocate,
    SCRIPT__vt__ilt__buffer_to_register,
    SCRIPT__vt__ilt__register_to_register,
    SCRIPT__vt__ilt__register_to_buffer,
    SCRIPT__vt__ilt__operate,
    SCRIPT__vt__ilt__read_file_into_buffer,
    SCRIPT__vt__ilt__write_buffer_into_file,
    SCRIPT__vt__ilt__call_new_byte_machine,
    SCRIPT__vt__ilt__debug__print_register,
    SCRIPT__vt__ilt__debug__print_character,
    SCRIPT__vt__ilt__debug__print_buffer,
    SCRIPT__vt__ilt__debug__print_all_registers,
    SCRIPT__vt__instruction_count,
    SCRIPT__vt__operation_count,
    SCRIPT__vt__error_count,
    SCRIPT__vt__program_buffer_address,
    SCRIPT__vt__program_buffer_length,
    SCRIPT__vt__input_buffer_address,
    SCRIPT__vt__input_buffer_length,
    SCRIPT__vt__debug_buffer_address,
    SCRIPT__vt__debug_buffer_length,
    SCRIPT__vt__register_size,
    SCRIPT__vt__address_size,
    SCRIPT__vt__buffer_length_size,
    SCRIPT__vt__values_count,
    SCRIPT__vt__LENGTH,
} SCRIPT__vt;

void SCRIPT__write__values_table(SCRIPT__value* table, BASIC__buffer program_buffer, BASIC__buffer input_buffer, BASIC__buffer debug_buffer) {
    // create values
    table[SCRIPT__vt__zero] = (SCRIPT__value)0;
    table[SCRIPT__vt__one] = (SCRIPT__value)1;
    table[SCRIPT__vt__ilt__quit] = (SCRIPT__value)SCRIPT__ilt__quit;
    table[SCRIPT__vt__ilt__get_value] = (SCRIPT__value)SCRIPT__ilt__get_value;
    table[SCRIPT__vt__ilt__write_register] = (SCRIPT__value)SCRIPT__ilt__write_register;
    table[SCRIPT__vt__ilt__allocate] = (SCRIPT__value)SCRIPT__ilt__allocate;
    table[SCRIPT__vt__ilt__deallocate] = (SCRIPT__value)SCRIPT__ilt__deallocate;
    table[SCRIPT__vt__ilt__buffer_to_register] = (SCRIPT__value)SCRIPT__ilt__buffer_to_register;
    table[SCRIPT__vt__ilt__register_to_register] = (SCRIPT__value)SCRIPT__ilt__register_to_register;
    table[SCRIPT__vt__ilt__register_to_buffer] = (SCRIPT__value)SCRIPT__ilt__register_to_buffer;
    table[SCRIPT__vt__ilt__operate] = (SCRIPT__value)SCRIPT__ilt__operate;
    table[SCRIPT__vt__ilt__read_file_into_buffer] = (SCRIPT__value)SCRIPT__ilt__read_file_into_buffer;
    table[SCRIPT__vt__ilt__write_buffer_into_file] = (SCRIPT__value)SCRIPT__ilt__write_buffer_into_file;
    table[SCRIPT__vt__ilt__call_new_byte_machine] = (SCRIPT__value)SCRIPT__ilt__call_new_byte_machine;
    table[SCRIPT__vt__ilt__debug__print_register] = (SCRIPT__value)SCRIPT__ilt__debug__print_register;
    table[SCRIPT__vt__ilt__debug__print_character] = (SCRIPT__value)SCRIPT__ilt__debug__print_character;
    table[SCRIPT__vt__ilt__debug__print_buffer] = (SCRIPT__value)SCRIPT__ilt__debug__print_buffer;
    table[SCRIPT__vt__ilt__debug__print_all_registers] = (SCRIPT__value)SCRIPT__ilt__debug__print_all_registers;
    table[SCRIPT__vt__instruction_count] = (SCRIPT__value)SCRIPT__it__LENGTH;
    table[SCRIPT__vt__operation_count] = (SCRIPT__value)SCRIPT__ot__LENGTH;
    table[SCRIPT__vt__error_count] = (SCRIPT__value)SCRIPT__et__LENGTH;
    table[SCRIPT__vt__program_buffer_address] = (SCRIPT__value)(SCRIPT__max_integer_width_type)program_buffer.p_address;
    table[SCRIPT__vt__program_buffer_length] = (SCRIPT__value)program_buffer.p_length;
    table[SCRIPT__vt__input_buffer_address] = (SCRIPT__value)(SCRIPT__max_integer_width_type)input_buffer.p_address;
    table[SCRIPT__vt__input_buffer_length] = (SCRIPT__value)input_buffer.p_length;
    table[SCRIPT__vt__debug_buffer_address] = (SCRIPT__value)(SCRIPT__max_integer_width_type)debug_buffer.p_address;
    table[SCRIPT__vt__debug_buffer_length] = (SCRIPT__value)debug_buffer.p_length;
    table[SCRIPT__vt__register_size] = (SCRIPT__value)sizeof(SCRIPT__register);
    table[SCRIPT__vt__address_size] = (SCRIPT__value)sizeof(BASIC__address);
    table[SCRIPT__vt__buffer_length_size] = (SCRIPT__value)sizeof(BASIC__length);
    table[SCRIPT__vt__values_count] = (SCRIPT__value)SCRIPT__vt__LENGTH;

    return;
}

/* Instruction Function Aliases */
BASIC__bt SCRIPT__get__bit___flag(u8* address, SCRIPT__flag_ID bit_offset) {
    return SCRIPT__get__bit(address, (u8)bit_offset);
}

void SCRIPT__set__bit___flag(u8* address, SCRIPT__flag_ID bit_offset, BASIC__bt value) {
    SCRIPT__set__bit(address, bit_offset, value);

    return;
}

SCRIPT__instruction_ID SCRIPT__read__instruction_ID_from_buffer(BASIC__address address, SCRIPT__max_integer_width_type offset) {
    return (SCRIPT__instruction_ID)BASIC__read__buffer(address + offset, sizeof(SCRIPT__instruction_ID));
}

SCRIPT__register_ID SCRIPT__read__register_ID_from_buffer(BASIC__address address, SCRIPT__offset offset) {
    return (SCRIPT__register_ID)BASIC__read__buffer(address + offset, sizeof(SCRIPT__register_ID));
}

SCRIPT__register SCRIPT__read__register_value_from_buffer(BASIC__address address, SCRIPT__offset offset) {
    return (SCRIPT__register)BASIC__read__buffer(address + offset, sizeof(SCRIPT__register));
}

SCRIPT__byte_size SCRIPT__read__byte_size_from_buffer(BASIC__address address, SCRIPT__offset offset) {
    return (SCRIPT__byte_size)BASIC__read__buffer(address + offset, sizeof(SCRIPT__byte_size));
}

SCRIPT__flag_ID SCRIPT__read__flag_address_from_buffer(BASIC__address address, SCRIPT__offset offset) {
    return (SCRIPT__flag_ID)BASIC__read__buffer(address + offset, sizeof(SCRIPT__flag_ID));
}

SCRIPT__operation_ID SCRIPT__read__operation_ID_from_buffer(BASIC__address address, SCRIPT__offset offset) {
    return (SCRIPT__operation_ID)BASIC__read__buffer(address + offset, sizeof(SCRIPT__operation_ID));
}

SCRIPT__value_ID SCRIPT__read__value_ID_from_buffer(BASIC__address address, SCRIPT__offset offset) {
    return (SCRIPT__value_ID)BASIC__read__buffer(address + offset, sizeof(SCRIPT__value_ID));
}

void SCRIPT__write__instruction_ID_to_buffer(BASIC__address address, SCRIPT__offset offset, SCRIPT__instruction_ID value) {
    BASIC__write__buffer((BASIC__data)value, address + offset, sizeof(SCRIPT__instruction_ID));

    return;
}

void SCRIPT__write__register_ID_to_buffer(BASIC__address address, SCRIPT__offset offset, SCRIPT__register_ID value) {
    BASIC__write__buffer((BASIC__data)value, address + offset, sizeof(SCRIPT__register_ID));

    return;
}

void SCRIPT__write__register_value_to_buffer(BASIC__address address, SCRIPT__offset offset, SCRIPT__register value) {
    BASIC__write__buffer((BASIC__data)value, address + offset, sizeof(SCRIPT__register));

    return;
}

void SCRIPT__write__byte_size_to_buffer(BASIC__address address, SCRIPT__offset offset, SCRIPT__byte_size value) {
    BASIC__write__buffer((BASIC__data)value, address + offset, sizeof(SCRIPT__byte_size));

    return;
}

void SCRIPT__write__flag_address_to_buffer(BASIC__address address, SCRIPT__offset offset, SCRIPT__flag_ID value) {
    BASIC__write__buffer((BASIC__data)value, address + offset, sizeof(SCRIPT__flag_ID));

    return;
}

void SCRIPT__write__operation_ID_to_buffer(BASIC__address address, SCRIPT__offset offset, SCRIPT__operation_ID value) {
    BASIC__write__buffer((BASIC__data)value, address + offset, sizeof(SCRIPT__operation_ID));

    return;
}

void SCRIPT__write__value_ID_to_buffer(BASIC__address address, SCRIPT__offset offset, SCRIPT__value_ID value) {
    BASIC__write__buffer((BASIC__data)value, address + offset, sizeof(SCRIPT__value_ID));

    return;
}

/* Instruction Temporaries */
typedef struct SCRIPT__temporaries {
    SCRIPT__instruction_ID p_instruction_ID;
    SCRIPT__byte_size p_byte_size;
    SCRIPT__value_ID p_value_ID;
    SCRIPT__register_ID p_r1;
    SCRIPT__register_ID p_r2;
    SCRIPT__register_ID p_r3;
    SCRIPT__register_ID p_r4;
    SCRIPT__register_ID p_r5;
    SCRIPT__register_ID p_r6;
    SCRIPT__register_ID p_r7;
    SCRIPT__register_ID p_r8;
    SCRIPT__register_ID p_r9;
    SCRIPT__operation_ID p_operation_ID;
    SCRIPT__register p_write_value;
    BASIC__buffer p_buffer_1;
    BASIC__buffer p_buffer_2;
    BASIC__buffer p_buffer_3;
    BASIC__buffer p_buffer_4;
    SCRIPT__flag_ID p_bit_number;
    SCRIPT__et p_error_code;
} SCRIPT__temporaries;

/* Byte Machine */
BASIC__buffer SCRIPT__run(SCRIPT__et* error, BASIC__buffer program, BASIC__buffer input, BASIC__buffer debug) {
    BASIC__buffer output;
    SCRIPT__value values[SCRIPT__vt__LENGTH];
    SCRIPT__registers registers;
    SCRIPT__temporaries temporaries;
    SCRIPT__registers* rs;
    BASIC__bt buffer_error_occured;
    BASIC__bt file_open_error_occured;
    BASIC__bt file_io_error_occured;

    // set interpreter error to no error
    *error = SCRIPT__et__no_error;

    // setup errors
    buffer_error_occured = BASIC__bt__false;
    file_open_error_occured = BASIC__bt__false;
    file_io_error_occured = BASIC__bt__false;

    // setup values
    SCRIPT__write__values_table((SCRIPT__value*)&values, program, input, debug);

    // setup registers
    rs = &registers;
    SCRIPT__write__register_with_address(rs, SCRIPT__rt__instruction_address, program.p_address);

    // process instructions
    while (BASIC__bt__true) {
        // read next instruction ID
        temporaries.p_instruction_ID = SCRIPT__read__instruction_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], 0);

        // process instructions
        switch (temporaries.p_instruction_ID) {
        case SCRIPT__it__quit:
            // get output buffer
            output.p_address = registers.p_registers[SCRIPT__rt__output_buffer_address];
            output.p_length = (BASIC__length)registers.p_registers[SCRIPT__rt__output_buffer_length];

            goto SCRIPT__label__quit;
        case SCRIPT__it__get_value:
            // read parameters
            temporaries.p_value_ID = SCRIPT__read__value_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__value_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__get_value;

            // get value
            if ((u64)temporaries.p_value_ID < SCRIPT__vt__LENGTH) {
                registers.p_registers[temporaries.p_r1] = (SCRIPT__register)values[(u64)temporaries.p_value_ID];
            } else {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__invalid_value_ID;
            }

            break;
        case SCRIPT__it__write_register:
            // read parameters
            temporaries.p_write_value = SCRIPT__read__register_value_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__write_register;

            // overwrite register
            registers.p_registers[temporaries.p_r1] = (SCRIPT__register)temporaries.p_write_value;

            break;
        case SCRIPT__it__allocate:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r3 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__allocate;

            // create buffer
            temporaries.p_buffer_1 = BASIC__create__buffer((BASIC__length)registers.p_registers[temporaries.p_r1]);
            if (temporaries.p_buffer_1.p_address == 0) {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__buffer_allocation_failure;
            }

            // set registers to hold buffer
            registers.p_registers[temporaries.p_r2] = temporaries.p_buffer_1.p_address;
            registers.p_registers[temporaries.p_r3] = (SCRIPT__register)temporaries.p_buffer_1.p_length;

            break;
        case SCRIPT__it__deallocate:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__deallocate;

            // get buffer
            temporaries.p_buffer_1.p_address = registers.p_registers[temporaries.p_r1];
            temporaries.p_buffer_1.p_length = (BASIC__length)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2];
            
            // destroy buffer
            BASIC__destroy__buffer(temporaries.p_buffer_1);

            break;
        case SCRIPT__it__buffer_to_register:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_byte_size = SCRIPT__read__byte_size_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__byte_size));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__buffer_to_register;

            // if byte size is valid
            if (temporaries.p_byte_size < 9) {
                // read from buffer to register
                registers.p_registers[temporaries.p_r2] = (SCRIPT__register)(SCRIPT__max_integer_width_type)BASIC__read__buffer(registers.p_registers[temporaries.p_r1], temporaries.p_byte_size);
            } else {
                // set error
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__invalid_byte_size;
            }

            break;
        case SCRIPT__it__register_to_register:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__register_to_register;

            // copy register
            registers.p_registers[temporaries.p_r2] = registers.p_registers[temporaries.p_r1];

            break;
        case SCRIPT__it__register_to_buffer:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_byte_size = SCRIPT__read__byte_size_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__byte_size));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__register_to_buffer;

            // write from register to buffer
            if (temporaries.p_byte_size < 9) {
                BASIC__write__buffer((BASIC__data)registers.p_registers[temporaries.p_r2], (BASIC__address)registers.p_registers[temporaries.p_r1], temporaries.p_byte_size);
            } else {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__invalid_byte_size;
            }

            break;
        case SCRIPT__it__operate:
            // read parameters
            temporaries.p_bit_number = SCRIPT__read__flag_address_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_operation_ID = SCRIPT__read__operation_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID));
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID) + sizeof(SCRIPT__operation_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID) + sizeof(SCRIPT__operation_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r3 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID) + sizeof(SCRIPT__operation_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__operate;

            // if desired flag enabled
            if (SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), temporaries.p_bit_number) == BASIC__bt__true) {
                // operate
                switch (temporaries.p_operation_ID) {
                case SCRIPT__ot__integer__add:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] + (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__integer__subtract:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] - (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__integer__multiply:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] * (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__integer__divide:
                    if ((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2] != 0) {
                        registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] / (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    } else {
                        registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__divide_by_zero;
                    }
                    break;
                case SCRIPT__ot__integer__modulo:
                    if ((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2] != 0) {
                        registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] % (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    } else {
                        registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__modulo_by_zero;
                    }
                    break;
                case SCRIPT__ot__binary__left_shift:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] << (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__binary__right_shift:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] >> (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__binary__not:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)(~(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]);
                    break;
                case SCRIPT__ot__binary__or:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] | (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__binary__and:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] & (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__binary__xor:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] ^ (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__binary__wipe_bits:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)((~(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]) & (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__flag__and:
                    SCRIPT__set__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r3], SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]) & SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]));
                    break;
                case SCRIPT__ot__flag__or:
                    SCRIPT__set__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r3], SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]) | SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]));
                    break;
                case SCRIPT__ot__flag__not:
                    SCRIPT__set__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r3], !SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]));
                    break;
                case SCRIPT__ot__flag__xor:
                    SCRIPT__set__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r3], SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]) ^ SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]));
                    break;
                case SCRIPT__ot__flag__write_flag:
                    SCRIPT__set__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r3], (BASIC__bt)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] != 0);
                    break;
                case SCRIPT__ot__flag__move_flag:
                    SCRIPT__set__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r3], SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]));
                    break;
                case SCRIPT__ot__flag__get_flag:
                    registers.p_registers[(SCRIPT__flag_ID)temporaries.p_r3] = (SCRIPT__register)(SCRIPT__max_integer_width_type)SCRIPT__get__bit___flag((u8*)&(registers.p_registers[SCRIPT__rt__flags_0]), (SCRIPT__flag_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]);
                    break;
                case SCRIPT__ot__integer__less_than:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)(SCRIPT__max_integer_width_type)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] < (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__integer__not_equal_to:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)(SCRIPT__max_integer_width_type)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] != (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__integer__equal_to:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)(SCRIPT__max_integer_width_type)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] == (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__integer__greater_than:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)(SCRIPT__max_integer_width_type)((SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1] > (SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r2]);
                    break;
                case SCRIPT__ot__static__register_to_register:
                    registers.p_registers[temporaries.p_r3] = (SCRIPT__register)registers.p_registers[temporaries.p_r1];
                    break;
                case SCRIPT__ot__dynamic__register_to_register:
                    registers.p_registers[(SCRIPT__register_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r3]] = registers.p_registers[(SCRIPT__register_ID)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]];
                    break;
                case SCRIPT__ot__LENGTH:
                default:
                    registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__invalid_operation;

                    goto SCRIPT__label__quit;
                }
            }

            break;
        case SCRIPT__it__read_file_into_buffer:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r3 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r4 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__read_file_into_buffer;

            // setup temporary buffer
            temporaries.p_buffer_1.p_address = registers.p_registers[temporaries.p_r1];
            temporaries.p_buffer_1.p_length = (BASIC__length)registers.p_registers[temporaries.p_r2];

            // get file
            temporaries.p_buffer_2 = BASIC__create__buffer_from_file(&buffer_error_occured, &file_open_error_occured, &file_io_error_occured, temporaries.p_buffer_1);
            if (buffer_error_occured == BASIC__bt__true) {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__buffer_allocation_failure;
            }
            if (file_open_error_occured == BASIC__bt__true) {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__invalid_file_address;
            }
            if (file_io_error_occured == BASIC__bt__true) {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__unable_to_fully_read_file_into_buffer;
            }

            // set registers
            registers.p_registers[temporaries.p_r3] = temporaries.p_buffer_2.p_address;
            registers.p_registers[temporaries.p_r4] = (SCRIPT__register)temporaries.p_buffer_2.p_length;

            break;
        case SCRIPT__it__write_buffer_into_file:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r3 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r4 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__write_buffer_into_file;

            // setup temporary buffer
            temporaries.p_buffer_1.p_address = registers.p_registers[temporaries.p_r1];
            temporaries.p_buffer_1.p_length = (BASIC__length)registers.p_registers[temporaries.p_r2];
            temporaries.p_buffer_2.p_address = registers.p_registers[temporaries.p_r3];
            temporaries.p_buffer_2.p_length = (BASIC__length)registers.p_registers[temporaries.p_r4];

            // write file
            BASIC__create__file_from_buffer(&buffer_error_occured, &file_open_error_occured, &file_io_error_occured, temporaries.p_buffer_1, temporaries.p_buffer_2);
            if (buffer_error_occured == BASIC__bt__true) {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__buffer_allocation_failure;
            }
            if (file_open_error_occured == BASIC__bt__true) {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__invalid_file_address;
            }
            if (file_io_error_occured == BASIC__bt__true) {
                registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__unable_to_fully_write_buffer_into_file;
            }

            break;
        case SCRIPT__it__call_new_byte_machine:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r3 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r4 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r5 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r6 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r7 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r8 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));
            temporaries.p_r9 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__call_new_byte_machine;

            // setup temporary buffers
            temporaries.p_buffer_1.p_address = registers.p_registers[temporaries.p_r1];
            temporaries.p_buffer_1.p_length = (BASIC__length)registers.p_registers[temporaries.p_r2];
            temporaries.p_buffer_2.p_address = registers.p_registers[temporaries.p_r3];
            temporaries.p_buffer_2.p_length = (BASIC__length)registers.p_registers[temporaries.p_r4];
            temporaries.p_buffer_3.p_address = registers.p_registers[temporaries.p_r5];
            temporaries.p_buffer_3.p_length = (BASIC__length)registers.p_registers[temporaries.p_r6];

            // call new byte machine
            temporaries.p_buffer_4 = SCRIPT__run(&(temporaries.p_error_code), temporaries.p_buffer_1, temporaries.p_buffer_2, temporaries.p_buffer_3);

            // create error code output
            registers.p_registers[temporaries.p_r7] = (SCRIPT__register)temporaries.p_error_code;

            // create output buffer
            registers.p_registers[temporaries.p_r8] = temporaries.p_buffer_4.p_address;
            registers.p_registers[temporaries.p_r9] = (SCRIPT__register)temporaries.p_buffer_4.p_length;

            break;
        case SCRIPT__it__debug__print_register:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__debug__print_register;

            // print register
            SCRIPT__print__register_as_number(registers.p_registers[temporaries.p_r1]);
            
            break;
        case SCRIPT__it__debug__print_character:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__debug__print_character;

            // print register
            putchar((u8)(SCRIPT__max_integer_width_type)registers.p_registers[temporaries.p_r1]);
            
            break;
        case SCRIPT__it__debug__print_buffer:
            // read parameters
            temporaries.p_r1 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID));
            temporaries.p_r2 = SCRIPT__read__register_ID_from_buffer(registers.p_registers[SCRIPT__rt__instruction_address], sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID));

            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__debug__print_buffer;

            // get buffer
            temporaries.p_buffer_1.p_address = registers.p_registers[temporaries.p_r1];
            temporaries.p_buffer_1.p_length = (BASIC__length)registers.p_registers[temporaries.p_r2];

            // print
            for (SCRIPT__max_integer_width_type i = 0; i < temporaries.p_buffer_1.p_length; i++) {
                putchar(((u8*)temporaries.p_buffer_1.p_address)[i]);
            }

            break;
        case SCRIPT__it__debug__print_all_registers:
            // adjust instruction pointer
            registers.p_registers[SCRIPT__rt__instruction_address] = registers.p_registers[SCRIPT__rt__instruction_address] + (SCRIPT__max_integer_width_type)SCRIPT__ilt__debug__print_all_registers;

            // print all registers
            SCRIPT__print__all_registers(registers);

            break;
        case SCRIPT__it__LENGTH:
        default:
            // set error in register
            registers.p_registers[SCRIPT__rt__error] = (SCRIPT__register)SCRIPT__et__invalid_instruction_ID;

            // quit
            goto SCRIPT__label__quit;
        }

        // reset temps
        buffer_error_occured = BASIC__bt__false;
        file_open_error_occured = BASIC__bt__false;
        file_io_error_occured = BASIC__bt__false;
    }

    SCRIPT__label__quit:

    // setup error return as code in error register
    *error = (SCRIPT__et)registers.p_registers[SCRIPT__rt__error];

    return output;
}

/* Instruction Creation */
SCRIPT__instruction_length SCRIPT__create__instruction__quit(BASIC__address destination) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__quit);

    return SCRIPT__ilt__quit;
}

SCRIPT__instruction_length SCRIPT__create__instruction__get_value(BASIC__address destination, SCRIPT__value_ID source_value_ID, SCRIPT__register_ID destination_value_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__get_value);
    SCRIPT__write__value_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), source_value_ID);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__value_ID), destination_value_register);

    return SCRIPT__ilt__get_value;
}

SCRIPT__instruction_length SCRIPT__create__instruction__write_register(BASIC__address destination, SCRIPT__register value, SCRIPT__register_ID destination_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__write_register);
    SCRIPT__write__register_value_to_buffer(destination, sizeof(SCRIPT__instruction_ID), value);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register), destination_register);

    return SCRIPT__ilt__write_register;
}

SCRIPT__instruction_length SCRIPT__create__instruction__allocate(BASIC__address destination, SCRIPT__register_ID allocation_size_source_register, SCRIPT__register_ID buffer_address_destination_register, SCRIPT__register_ID buffer_length_destination_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__allocate);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), allocation_size_source_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), buffer_address_destination_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), buffer_length_destination_register);

    return SCRIPT__ilt__allocate;
}

SCRIPT__instruction_length SCRIPT__create__instruction__deallocate(BASIC__address destination, SCRIPT__register_ID buffer_address_source_register, SCRIPT__register_ID buffer_length_source_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__deallocate);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), buffer_address_source_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), buffer_length_source_register);

    return SCRIPT__ilt__deallocate;
}

SCRIPT__instruction_length SCRIPT__create__instruction__buffer_to_register(BASIC__address destination, SCRIPT__register_ID buffer_address_source_register, SCRIPT__byte_size byte_size, SCRIPT__register_ID destination_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__buffer_to_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), buffer_address_source_register);
    SCRIPT__write__byte_size_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), byte_size);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__byte_size), destination_register);

    return SCRIPT__ilt__buffer_to_register;
}

SCRIPT__instruction_length SCRIPT__create__instruction__register_to_register(BASIC__address destination, SCRIPT__register_ID source_register, SCRIPT__register_ID destination_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__register_to_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), source_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), destination_register);

    return SCRIPT__ilt__register_to_register;
}

SCRIPT__instruction_length SCRIPT__create__instruction__register_to_buffer(BASIC__address destination, SCRIPT__register_ID source_register, SCRIPT__byte_size byte_size, SCRIPT__register_ID buffer_address_destination_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__register_to_buffer);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), source_register);
    SCRIPT__write__byte_size_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), byte_size);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__byte_size), buffer_address_destination_register);

    return SCRIPT__ilt__register_to_buffer;
}

SCRIPT__instruction_length SCRIPT__create__instruction__operate(BASIC__address destination, SCRIPT__flag_ID flag_bit, SCRIPT__operation_ID operation, SCRIPT__register_ID operand_register_1, SCRIPT__register_ID operand_register_2, SCRIPT__register_ID operation_result_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__operate);
    SCRIPT__write__flag_address_to_buffer(destination, sizeof(SCRIPT__instruction_ID), flag_bit);
    SCRIPT__write__operation_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID), operation);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID) + sizeof(SCRIPT__operation_ID), operand_register_1);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID) + sizeof(SCRIPT__operation_ID) + sizeof(SCRIPT__register_ID), operand_register_2);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__flag_ID) + sizeof(SCRIPT__operation_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), operation_result_register);

    return SCRIPT__ilt__operate;
}

SCRIPT__instruction_length SCRIPT__create__instruction__read_file_into_buffer(BASIC__address destination, SCRIPT__register_ID source_buffer_address_register, SCRIPT__register_ID source_buffer_length_register, SCRIPT__register_ID destination_buffer_address_register, SCRIPT__register_ID destination_buffer_length_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__read_file_into_buffer);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), source_buffer_address_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), source_buffer_length_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), destination_buffer_address_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), destination_buffer_length_register);

    return SCRIPT__ilt__read_file_into_buffer;
}

SCRIPT__instruction_length SCRIPT__create__instruction__write_buffer_into_file(BASIC__address destination, SCRIPT__register_ID source_buffer_address_register, SCRIPT__register_ID source_buffer_length_register, SCRIPT__register_ID file_name_buffer_address_register, SCRIPT__register_ID file_name_buffer_length_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__write_buffer_into_file);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), source_buffer_address_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), source_buffer_length_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), file_name_buffer_address_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), file_name_buffer_length_register);

    return SCRIPT__ilt__write_buffer_into_file;
}

SCRIPT__instruction_length SCRIPT__create__instruction__call_new_byte_machine(BASIC__address destination, SCRIPT__register_ID program_buffer_address_register, SCRIPT__register_ID program_buffer_length_register, SCRIPT__register_ID input_buffer_address_register, SCRIPT__register_ID input_buffer_length_register, SCRIPT__register_ID debug_buffer_address_register, SCRIPT__register_ID debug_buffer_length_register, SCRIPT__register_ID error_destination_register, SCRIPT__register_ID output_buffer_address_register, SCRIPT__register_ID output_buffer_length_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__call_new_byte_machine);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), program_buffer_address_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), program_buffer_length_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), input_buffer_address_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), input_buffer_length_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), debug_buffer_address_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), debug_buffer_length_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), error_destination_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), output_buffer_address_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID) + sizeof(SCRIPT__register_ID), output_buffer_length_register);

    return SCRIPT__ilt__call_new_byte_machine;
}

SCRIPT__instruction_length SCRIPT__create__instruction__debug__print_register(BASIC__address destination, SCRIPT__register_ID source_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__debug__print_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), source_register);

    return SCRIPT__ilt__debug__print_register;
}

SCRIPT__instruction_length SCRIPT__create__instruction__debug__print_character(BASIC__address destination, SCRIPT__register_ID source_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__debug__print_character);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), source_register);

    return SCRIPT__ilt__debug__print_character;
}

SCRIPT__instruction_length SCRIPT__create__instruction__debug__print_buffer(BASIC__address destination, SCRIPT__register_ID buffer_address_source_register, SCRIPT__register_ID buffer_length_source_register) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__debug__print_buffer);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID), buffer_address_source_register);
    SCRIPT__write__register_ID_to_buffer(destination, sizeof(SCRIPT__instruction_ID) + sizeof(SCRIPT__register_ID), buffer_length_source_register);

    return SCRIPT__ilt__debug__print_buffer;
}

SCRIPT__instruction_length SCRIPT__create__instruction__debug__print_all_registers(BASIC__address destination) {
    SCRIPT__write__instruction_ID_to_buffer(destination, 0, SCRIPT__it__debug__print_all_registers);

    return SCRIPT__ilt__debug__print_all_registers;
}

#endif
