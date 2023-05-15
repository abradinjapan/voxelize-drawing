#ifndef VOXELIZE__simulation
#define VOXELIZE__simulation

/* Include */
// game
#include "../base/basic.h"

/* Define */
// coordinates
typedef u64 SIM__coordinate;
typedef SIM__coordinate SIM__coordinate_x;
typedef SIM__coordinate SIM__coordinate_y;
typedef SIM__coordinate SIM__coordinate_z;

// hitboxes
typedef u8 SIM__hitbox_type;
typedef u64 SIM__hitbox_index;
typedef SIM__hitbox_index SIM__hitbox_count;

// the amount of bits that represent the in-block position
#define SIM__coordinate_bits_per_block 16

/* Position */
// the information necessary to maintain a global position
typedef struct SIM__position {
    SIM__coordinate_x p_x;
    SIM__coordinate_y p_y;
    SIM__coordinate_z p_z;
} SIM__position;

// create a position
SIM__position SIM__create__position(SIM__coordinate_x x, SIM__coordinate_y y, SIM__coordinate_z z) {
    SIM__position output;

    // setup output
    output.p_x = x;
    output.p_y = y;
    output.p_z = z;

    return output;
}

/* Hitbox */
// hitbox type
typedef enum SIM__ht {
    SIM__ht__none,
    SIM__ht__solid__cube,

    SIM__ht__COUNT,
} SIM__ht;

// hitbox
typedef struct SIM__hitbox {
    SIM__hitbox_type p_type;
} SIM__hitbox;

// hitboxes
typedef struct SIM__hitboxes {
    BASIC__buffer p_block_hitboxes;
    SIM__hitbox_count p_block_hitbox_count;
} SIM__hitboxes;

// create hitbox
SIM__hitbox SIM__create__hitbox(SIM__ht type) {
    SIM__hitbox output;

    // setup output
    output.p_type = type;

    return output;
}

// create hitboxes
SIM__hitboxes SIM__open__hitboxes(SIM__hitbox_count block_hitbox_count) {
    SIM__hitboxes output;

    // setup output
    output.p_block_hitboxes = BASIC__create__buffer(block_hitbox_count * sizeof(SIM__hitbox));
    output.p_block_hitbox_count = block_hitbox_count;

    return output;
}

// destroy hitboxes
void SIM__close__hitboxes(SIM__hitboxes hitboxes) {
    // close buffers
    BASIC__destroy__buffer(hitboxes.p_block_hitboxes);

    return;
}

// write hitbox
void SIM__write__hitbox(SIM__hitboxes hitboxes, SIM__hitbox_index index, SIM__hitbox hitbox) {
    // write data
    ((SIM__hitbox*)hitboxes.p_block_hitboxes.p_address)[index] = (SIM__hitbox)hitbox;

    return;
}

// read hitbox
SIM__hitbox SIM__read__hitbox(SIM__hitboxes hitboxes, SIM__hitbox_index index) {
    return ((SIM__hitbox*)hitboxes.p_block_hitboxes.p_address)[index];
}

/* Simulation */
// the game simulation
typedef struct SIM__simulation {
    SIM__position p_player_position;
} SIM__simulation;

#endif
