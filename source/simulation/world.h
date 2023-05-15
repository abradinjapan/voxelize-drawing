#ifndef VOXELIZE__world
#define VOXELIZE__world

/* Include */
#include "../base/basic.h"

/* Define */
// coordinates
typedef u64 WORLD__coordinate;
typedef WORLD__coordinate WORLD__coordinate_x;
typedef WORLD__coordinate WORLD__coordinate_y;
typedef WORLD__coordinate WORLD__coordinate_z;

// the amount of bits that represent the in-block position
#define SIM__coordinate_bits_per_block 16

// the data container of one block (block type ID but also unique block meta data)
typedef u64 WORLD__block_data;

// the index of a block in the one dimensional chunk blocks array
typedef u16 WORLD__block_index;

// the block ID
typedef u64 WORLD__block_ID;

// the sides of a chunk
typedef u8 WORLD__chunk_axis_index;
typedef WORLD__chunk_axis_index WORLD__chunk_x;
typedef WORLD__chunk_axis_index WORLD__chunk_y;
typedef WORLD__chunk_axis_index WORLD__chunk_z;

// the size of one chunk dimension in blocks
#define WORLD__chunk_side_block_count 16

// the size of two chunk dimensions in blocks
#define WORLD__chunk_slice_block_count (WORLD__chunk_side_block_count*WORLD__chunk_side_block_count)

// the total amount of blocks in a chunk (all sides are always the same length)
#define WORLD__chunk_total_block_count (WORLD__chunk_side_block_count*WORLD__chunk_side_block_count*WORLD__chunk_side_block_count)

// the index of a chunk into a 3d chunk array
typedef u32 WORLD__chunks_index;

// the sides of the chunks
typedef u8 WORLD__chunks_axis_index;
typedef WORLD__chunks_axis_index WORLD__chunks_x;
typedef WORLD__chunks_axis_index WORLD__chunks_y;
typedef WORLD__chunks_axis_index WORLD__chunks_z;

/* Position */
// the information necessary to maintain a global position
typedef struct WORLD__position {
    WORLD__coordinate_x p_x;
    WORLD__coordinate_y p_y;
    WORLD__coordinate_z p_z;
} WORLD__position;

// create a position
WORLD__position WORLD__create__position(WORLD__coordinate_x x, WORLD__coordinate_y y, WORLD__coordinate_z z) {
    WORLD__position output;

    // setup output
    output.p_x = x;
    output.p_y = y;
    output.p_z = z;

    return output;
}

// create a null position
WORLD__position WORLD__create_null__position() {
    return WORLD__create__position(0, 0, 0);
}

/* Block */
// get block type from block data
WORLD__block_ID WORLD__get__block_ID_from_block_data(WORLD__block_data block_data) {
    // extract block type and return
    return (WORLD__block_ID)block_data;
}

// create block data from raw block type
WORLD__block_data WORLD__create__block(WORLD__block_ID block_ID) {
    // setup the data for the block (can be more complex later as block data has other states packed in)
    return (WORLD__block_data)block_ID;
}

// create empty block
WORLD__block_data WORLD__create_null__block() {
    // return empty
    return WORLD__create__block(0);
}

/* Chunk */
typedef struct WORLD__chunk {
    WORLD__block_data p_blocks[WORLD__chunk_total_block_count];
    WORLD__position p_global_position;
} WORLD__chunk;

// chunk address
typedef WORLD__chunk* WORLD__chunk_address;

// create a chunk with one specific block data for every block
WORLD__chunk WORLD__create__chunk(WORLD__block_data block_data, WORLD__position global_position) {
    WORLD__chunk output;

    // setup output
    for (WORLD__block_index block_index = 0; block_index < WORLD__chunk_total_block_count; block_index++) {
        // set block
        output.p_blocks[block_index] = block_data;
    }
    output.p_global_position = global_position;
    
    return output;
}

// create a blank chunk
WORLD__chunk WORLD__create_null__chunk() {
    // create empty chunk
    return WORLD__create__chunk(WORLD__create_null__block(), WORLD__create_null__position());
}

// create chunk where every other block is air and one custom block type
WORLD__chunk WORLD__create__chunk__alternating_block_pattern(WORLD__block_ID first_block, WORLD__block_ID second_block, WORLD__position global_position) {
    WORLD__chunk output;

    // setup output to all first type
    output = WORLD__create__chunk(WORLD__create__block(first_block), global_position);

    // setup every even block to be the chosen block type
    for (WORLD__block_index block_index = 0; block_index < WORLD__chunk_total_block_count; block_index += 2) {
        // set block to second type
        output.p_blocks[block_index] = WORLD__create__block(second_block);
    }
    output.p_global_position = global_position;

    return output;
}

// create chunk where every 3 blocks are a custom type
WORLD__chunk WORLD__create__chunk__3_rotating_block_pattern(WORLD__block_data first_block, WORLD__block_data second_block, WORLD__block_data third_block, WORLD__position global_position) {
    WORLD__chunk output;

    // setup output to all first type
    output = WORLD__create__chunk(WORLD__create__block(first_block), global_position);

    // setup every even block to be the chosen block type
    for (WORLD__block_index block_index = 0; block_index < WORLD__chunk_total_block_count - 3; block_index += 3) {
        // set blocks
        output.p_blocks[block_index + 0] = first_block;
        output.p_blocks[block_index + 1] = second_block;
        output.p_blocks[block_index + 2] = third_block;
    }
    output.p_global_position = global_position;

    return output;
}

// create chunk with 6 of one type and another seventh in a rotating pattern
WORLD__chunk WORLD__create__chunk__7_rotating_block_pattern(WORLD__block_data first_six_blocks, WORLD__block_data seventh_block, WORLD__position global_position) {
    WORLD__chunk output;

    // setup output to all first type
    output = WORLD__create__chunk(WORLD__create__block(first_six_blocks), global_position);

    // setup every seventh block to be the chosen block type
    for (WORLD__block_index block_index = 0; block_index < WORLD__chunk_total_block_count - 7; block_index += 7) {
        // set blocks
        output.p_blocks[block_index] = seventh_block;
    }
    output.p_global_position = global_position;

    return output;
}

// calculate block index from the block's chunk internal coords
WORLD__block_index WORLD__calculate__block_index(WORLD__chunk_x block_x, WORLD__chunk_y block_y, WORLD__chunk_z block_z) {
    // calculate (z is forward and back, y is up and down, x is left and right)
    return (block_z * WORLD__chunk_slice_block_count) + (block_y * WORLD__chunk_side_block_count) + block_x;
}

// set block
void WORLD__set__block_data_from_chunk_address(WORLD__chunk_address chunk_address, WORLD__block_index block_index, WORLD__block_data block_data) {
    // set data
    (*chunk_address).p_blocks[block_index] = block_data;

    return;
}

// get block
WORLD__block_data WORLD__get__block_data_from_chunk_address(WORLD__chunk_address chunk_address, WORLD__block_index block_index) {
    // return data
    return (*chunk_address).p_blocks[block_index];
}

// set position
void WORLD__set__chunk_position_from_chunk_address(WORLD__chunk_address chunk_address, WORLD__position position) {
    // set data
    (*chunk_address).p_global_position = position;

    return;
}

// get position
WORLD__position WORLD__get__chunk_position_from_chunk_address(WORLD__chunk_address chunk_address) {
    // get data
    return (*chunk_address).p_global_position;
}

/* Chunks */
typedef struct WORLD__chunks {
    BASIC__buffer p_chunk_block_data;
    WORLD__chunks_x p_width;
    WORLD__chunks_y p_height;
    WORLD__chunks_z p_depth;
} WORLD__chunks;

// close chunks
void WORLD__close__chunks(WORLD__chunks chunks) {
    // close buffer data
    BASIC__destroy__buffer(chunks.p_chunk_block_data);

    return;
}

// setup chunks
WORLD__chunks WORLD__create__chunks(BASIC__buffer chunk_block_data, WORLD__chunks_x width, WORLD__chunks_y height, WORLD__chunks_z depth) {
    WORLD__chunks output;

    // setup output
    output.p_chunk_block_data = chunk_block_data;
    output.p_width = width;
    output.p_height = height;
    output.p_depth = depth;

    return output;
}

// setup null chunks
WORLD__chunks WORLD__create_null__chunks() {
    // setup empty chunks
    return WORLD__create__chunks(BASIC__create_null__buffer(), 0, 0, 0);
}

// calculate chunk index from coords
WORLD__chunks_index WORLD__calculate__chunks_index(WORLD__chunks chunks, WORLD__chunks_x x, WORLD__chunks_y y, WORLD__chunks_z z) {
    // return chunk index
    return (chunks.p_height * chunks.p_width * z) + (chunks.p_width * y) + x;
}

// get chunk address
WORLD__chunk_address WORLD__get__chunk_pointer_in_chunks(WORLD__chunks chunks, WORLD__chunks_index chunks_index) {
    // return data
    return (WORLD__chunk_address)(chunks.p_chunk_block_data.p_address + (sizeof(WORLD__chunk) * chunks_index));
}

// set chunk by address
void WORLD__set__chunk_in_chunks(WORLD__chunks chunks, WORLD__chunks_index chunks_index, WORLD__chunk_address new_chunk_address) {
    WORLD__chunk_address setting_chunk_address;

    // get chunk
    setting_chunk_address = WORLD__get__chunk_pointer_in_chunks(chunks, chunks_index);

    // copy block data
    for (WORLD__block_index block_index = 0; block_index < WORLD__chunk_total_block_count; block_index++) {
        (*setting_chunk_address).p_blocks[block_index] = (*new_chunk_address).p_blocks[block_index];
    }

    return;
}

// fill already allocated chunks
void WORLD__setup__chunks(WORLD__chunks chunks, WORLD__chunks_x width, WORLD__chunks_y height, WORLD__chunks_z depth, WORLD__chunk_address default_chunk_address) {
    WORLD__chunks_index chunk_count;

    // setup chunk index
    chunk_count = width * height * depth;

    // write default chunk to all chunks
    for (WORLD__chunks_index chunk_index = 0; chunk_index < chunk_count; chunk_index += 1) {
        // write chunk
        WORLD__set__chunk_in_chunks(chunks, chunk_index, default_chunk_address);
    }

    return;
}

// allocate and fill chunks
WORLD__chunks WORLD__open__chunks(WORLD__chunks_x width, WORLD__chunks_y height, WORLD__chunks_z depth, WORLD__chunk_address default_chunk) {
    WORLD__chunks output;

    // setup output to null
    output = WORLD__create_null__chunks();

    // allocate chunk data
    output.p_chunk_block_data = BASIC__create__buffer(sizeof(WORLD__chunk) * width * height * depth);

    // if allocation denied, return null result
    if (output.p_chunk_block_data.p_address == 0) {
        // return null chunks
        return WORLD__create_null__chunks();
    }

    // setup size data
    output.p_width = width;
    output.p_height = height;
    output.p_depth = depth;

    // setup chunk data
    WORLD__setup__chunks(output, width, height, depth, default_chunk);

    return output;
}

/* World */
typedef struct WORLD__world {
    WORLD__position p_player_position;
    WORLD__chunks p_chunks;
} WORLD__world;

#endif
