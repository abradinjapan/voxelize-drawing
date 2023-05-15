#ifndef VOXELIZE__configuration_2
#define VOXELIZE__configuration_2

/* Include */
// game
#include "game.h"

// random number generator
#include <stdlib.h>

/* Define */
// gui face count
#define CONF2__gui__face_count 1

// gui faces
#define CONF2__gui__button 0

// block count
#define CONF2__block_count 8

// block types
#define CONF2__block__no_block 0
#define CONF2__block__air 1
#define CONF2__block__stone 2
#define CONF2__block__grass 3
#define CONF2__block__sand 4
#define CONF2__block__dirt 5
#define CONF2__block__glass 6
#define CONF2__block__tar 7

/* Setup Game Data */
// create game block faces
TEX__faces CONF2__open__block_faces() {
    TEX__faces output;
    BASIC__address output_faces_pointer;
    TEX__pixel_color color_intensity;

    // setup basic information
    output.p_width = 16;
    output.p_height = 16;
    output.p_count = CONF2__block_count;
    color_intensity = 5;

    // create buffer
    output.p_faces = BASIC__create__buffer(sizeof(TEX__pixel) * (output.p_height * output.p_width) * output.p_count);

    // setup random
    srand(1234567890);

    // setup face pointer
    output_faces_pointer = output.p_faces.p_address;

    // create no face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        for (u64 color = 0; color < TEX__pixel_colors_count; color++) {
            // write data
            *((TEX__pixel_color*)output_faces_pointer) = 0;

            // next color
            output_faces_pointer += sizeof(TEX__pixel_color);
        }
    }

    // create air face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write pixel
        for (u64 color = 0; color < TEX__pixel_colors_count; color++) {
            // write data
            *((TEX__pixel_color*)output_faces_pointer) = 0;

            // next color
            output_faces_pointer += sizeof(TEX__pixel_color);
        }
    }

    // create stone face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 120 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 120 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 120 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);
    }

    // create grass face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 50 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 240 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 50 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);
    }

    // create sand face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 242 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 214 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 136 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);
    }

    // create dirt face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 100 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 50 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 0 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);
    }

    // create glass face
    for (u64 pixel = 0; pixel < (output.p_width); pixel++) {
        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);
    }
    for (u64 pixel = 0; pixel < (output.p_height - 2); pixel++) {
        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);

        for (u64 row = 0; row < output.p_width - 2; row++) {
            // write red pixel
            *((TEX__pixel_color*)output_faces_pointer) = 255;
            output_faces_pointer += sizeof(TEX__pixel_color);

            // write green pixel
            *((TEX__pixel_color*)output_faces_pointer) = 255;
            output_faces_pointer += sizeof(TEX__pixel_color);

            // write blue pixel
            *((TEX__pixel_color*)output_faces_pointer) = 255;
            output_faces_pointer += sizeof(TEX__pixel_color);

            // write alpha pixel
            *((TEX__pixel_color*)output_faces_pointer) = 0;
            output_faces_pointer += sizeof(TEX__pixel_color);
        }

        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);
    }
    for (u64 pixel = 0; pixel < output.p_width; pixel++) {
        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 245;
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);
    }

    // create tar face
    for (u64 pixel = 0; pixel < (output.p_height * output.p_width); pixel++) {
        // write red pixel
        *((TEX__pixel_color*)output_faces_pointer) = 20 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);

        // write green pixel
        *((TEX__pixel_color*)output_faces_pointer) = 20 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write blue pixel
        *((TEX__pixel_color*)output_faces_pointer) = 20 + (rand() % color_intensity);
        output_faces_pointer += sizeof(TEX__pixel_color);
            
        // write alpha pixel
        *((TEX__pixel_color*)output_faces_pointer) = 255;
        output_faces_pointer += sizeof(TEX__pixel_color);
    }

    return output;
}

// create game skins
SKIN__skins CONF2__open__skins() {
    SKIN__skins output;
    SKIN__skin_count no_skin = CONF2__block__no_block;
    SKIN__skin_count air_skin = CONF2__block__air;
    SKIN__skin_count stone_skin = CONF2__block__stone;
    SKIN__skin_count grass_skin = CONF2__block__grass;
    SKIN__skin_count sand_skin = CONF2__block__sand;
    SKIN__skin_count dirt_skin = CONF2__block__dirt;
    SKIN__skin_count glass_skin = CONF2__block__glass;
    SKIN__skin_count tar_skin = CONF2__block__tar;

    // setup output
    output.p_block_skins = SKIN__open__block_skins(CONF2__block_count);
    output.p_block_skins_count = CONF2__block_count;

    // setup no block skin
    SKIN__set__skin__block(output, CONF2__block__no_block, SKIN__create__block__one_skin(no_skin, SKIN__bdt__dont_draw));
    SKIN__set__skin__block(output, CONF2__block__air, SKIN__create__block__one_skin(air_skin, SKIN__bdt__dont_draw));
    SKIN__set__skin__block(output, CONF2__block__stone, SKIN__create__block__one_skin(stone_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__block__grass, SKIN__create__block__one_skin(grass_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__block__sand, SKIN__create__block__one_skin(sand_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__block__dirt, SKIN__create__block__one_skin(dirt_skin, SKIN__bdt__draw_only_one_side));
    SKIN__set__skin__block(output, CONF2__block__glass, SKIN__create__block__one_skin(glass_skin, SKIN__bdt__draw_all_sides));
    SKIN__set__skin__block(output, CONF2__block__tar, SKIN__create__block__one_skin(tar_skin, SKIN__bdt__draw_only_one_side));

    return output;
}

/* Setup Configuration */
// opens a game in a specific way
GAME__information CONF2__open__game() {
    GAME__information output;
    BASIC__bt file_error;
    BASIC__buffer vertex_shader;
    BASIC__buffer fragment_shader;

    // setup variables
    output = GAME__create_null__game_information();
    vertex_shader = BASIC__open__buffer_from_file_as_c_string(&file_error, BASIC__create__c_string_reference("./shaders/chunks/vertex.glsl", BASIC__bt__true));
    fragment_shader = BASIC__open__buffer_from_file_as_c_string(&file_error, BASIC__create__c_string_reference("./shaders/chunks/fragment.glsl", BASIC__bt__true));

    // setup graphics
    output.p_graphics = WINDOW__open__window(&(output.p_graphics_error), WINDOW__create__window_configuration(BASIC__create__c_string_reference("Voxelize", BASIC__bt__false), 720, 480));
    if (output.p_graphics_error != WINDOW__et__no_error) {
        // setup error indicator
        output.p_error_occured = BASIC__bt__true;

        // print error information
        printf("WINDOW__et[ %lu ]\n", (u64)output.p_graphics_error);

        return output;
    }

    // setup shaders
    output.p_chunks_shader_program = SHADER__compile__shaders_program(&(output.p_shader_error), &(output.p_shader_error_log), vertex_shader, fragment_shader);
    if (output.p_shader_error != SHADER__et__no_error) {
        // setup error indicator
        output.p_error_occured = BASIC__bt__true;

        // print error information
        printf("SHADER__et[ %lu ]:\n%s\n", (u64)output.p_shader_error, (u8*)&(output.p_shader_error_log.p_log));

        return output;
    }
    SHADER__use__program(output.p_chunks_shader_program);

    // clean up shader source buffers
    BASIC__destroy__buffer(vertex_shader);
    BASIC__destroy__buffer(fragment_shader);

    // setup temporaries
    output.p_temporaries = RENDER__open__temporaries();

    // setup opengl basic settings
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    // setup controls
    output.p_controls = CONTROLS__open__controls();

    return output;
}

/*// open chunks in a test world
WORLD__chunks CONF2__create__test_world() {
    WORLD__chunks output;
    WORLD__chunks_index chunk_size = 2;
    WORLD__chunk temp_chunk;
    WORLD__block_data blocks[] = {
        CONF2__block__dirt, CONF2__block__grass,
        CONF2__block__grass, CONF2__block__sand,
        CONF2__block__sand, CONF2__block__glass,
        CONF2__block__glass, CONF2__block__stone,
        CONF2__block__grass, CONF2__block__stone,
        CONF2__block__sand, CONF2__block__glass,
        CONF2__block__dirt, CONF2__block__sand,
        CONF2__block__grass, CONF2__block__glass
    };

    // create chunk
    temp_chunk = WORLD__create__chunk__3_rotating_block_pattern(WORLD__create__block(CONF2__block__glass), WORLD__create__block(CONF2__block__air), WORLD__create__block(CONF2__block__sand));

    // create chunks
    output = WORLD__open__chunks(chunk_size, chunk_size, chunk_size, &temp_chunk);

    // create other unique chunks
    for (u64 x = 0; x < chunk_size; x++) {
        for (u64 y = 0; y < chunk_size; y++) {
            for (u64 z = 0; z < chunk_size; z++) {
                // create chunk
                temp_chunk = WORLD__create__chunk__3_rotating_block_pattern(WORLD__create__block(blocks[2 * WORLD__calculate__chunks_index(output, x, y, z)]), WORLD__create__block(blocks[2 * WORLD__calculate__chunks_index(output, x, y, z) + 1]), WORLD__create__block(CONF2__block__air));

                // setup chunk data
                WORLD__set__chunk_in_chunks(output, WORLD__calculate__chunks_index(output, x, y, z), &temp_chunk);
            }
        }
    }

    return output;
}*/

// create small world
WORLD__chunks CONF2__create__test_world_2() {
    WORLD__chunks output;
    WORLD__chunks_index chunks_size = 3;
    WORLD__chunk chunk;
    WORLD__chunk_address temp_chunk;

    // create blank chunk
    chunk = WORLD__create__chunk(WORLD__create__block(CONF2__block__air), WORLD__create_null__position());

    // setup chunks
    output = WORLD__open__chunks(chunks_size, chunks_size, chunks_size, &chunk);

    // build ground
    for (u64 chunks_x = 0; chunks_x < chunks_size; chunks_x++) {
        for (u64 chunks_z = 0; chunks_z < chunks_size; chunks_z++) {
            // get chunk
            temp_chunk = WORLD__get__chunk_pointer_in_chunks(output, WORLD__calculate__chunks_index(output, chunks_x, 0, chunks_z));

            // draw in one chunk
            for (u64 block_x = 0; block_x < WORLD__chunk_side_block_count; block_x++) {
                for (u64 block_z = 0; block_z < WORLD__chunk_side_block_count; block_z++) {
                    // setup stone
                    WORLD__set__block_data_from_chunk_address(temp_chunk, WORLD__calculate__block_index(block_x, 0, block_z), WORLD__create__block(CONF2__block__stone));

                    // setup topsoil
                    WORLD__set__block_data_from_chunk_address(temp_chunk, WORLD__calculate__block_index(block_x, 1, block_z), WORLD__create__block(CONF2__block__grass));
                }
            }
        }
    }

    // create middle chunk
    chunk = WORLD__create__chunk__3_rotating_block_pattern(WORLD__create__block(CONF2__block__sand), WORLD__create__block(CONF2__block__stone), WORLD__create__block(CONF2__block__grass), WORLD__create_null__position());
    WORLD__set__chunk_in_chunks(output, WORLD__calculate__chunks_index(output, 1, 0, 1), &chunk);

    // create other chunk
    chunk = WORLD__create__chunk__3_rotating_block_pattern(WORLD__create__block(CONF2__block__tar), WORLD__create__block(CONF2__block__air), WORLD__create__block(CONF2__block__air), WORLD__create_null__position());
    WORLD__set__chunk_in_chunks(output, WORLD__calculate__chunks_index(output, 2, 2, 2), &chunk);

    return output;
}

// setup game
void CONF2__setup__game(GAME__information* game_information) {
    // setup textures
    (*game_information).p_game_textures = TEX__open__game_textures(CONF2__open__block_faces(), (*game_information).p_chunks_shader_program);

    // setup skins
    (*game_information).p_skins = CONF2__open__skins();

    // setup chunks
    (*game_information).p_chunks = CONF2__create__test_world_2();

    // setup world
    (*game_information).p_world = RENDER__open__world((*game_information).p_chunks);

    // setup drawing information
    RENDER__render__world((*game_information).p_skins, (*game_information).p_chunks, (*game_information).p_world, (*game_information).p_temporaries);

    // fixate mouse
    CONTROLS__update__mouse_lock(BASIC__bt__true);

    return;
}

// closes the game properly
void CONF2__close__game(GAME__information game_information) {
    // clean up
    CONTROLS__close__controls(game_information.p_controls);
    TEX__close__game_textures(game_information.p_game_textures);
    SKIN__close__skins(game_information.p_skins);
    SHADER__close__program(game_information.p_chunks_shader_program);
    WINDOW__close__window(game_information.p_graphics);
    WORLD__close__chunks(game_information.p_chunks);
    RENDER__close__world(game_information.p_world);
    RENDER__close__temporaries(game_information.p_temporaries);

    return;
}

// displays the next frame
void CONF2__display__frame(GAME__information* game_information) {
    RENDER__transform transform_change;
    float movement_speed = 1.0f;

    // setup null change
    transform_change = RENDER__create__transform(RENDER__create__vertex(0.0f, 0.0f, 0.0f), RENDER__create__vertex(0.0f, 0.0f, 0.0f));

    // move objects
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_ESCAPE)) {
        (*game_information).p_game_state = GAME__gsi__game_quitting;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_A)) {
        transform_change.p_position_offset.p_vertices[0] += movement_speed * -1.0f;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_D)) {
        transform_change.p_position_offset.p_vertices[0] += movement_speed;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_LSHIFT)) {
        transform_change.p_position_offset.p_vertices[1] += movement_speed;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_SPACE)) {
        transform_change.p_position_offset.p_vertices[1] += movement_speed * -1.0f;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_W)) {
        transform_change.p_position_offset.p_vertices[2] += movement_speed * -1.0f;
    }
    if (CONTROLS__check__key_pressed((*game_information).p_controls, SDL_SCANCODE_S)) {
        transform_change.p_position_offset.p_vertices[2] += movement_speed;
    }
    CONTROLS__update__mouse_position_change(&((*game_information).p_controls));
    transform_change.p_rotation.p_vertices[0] = (*game_information).p_controls.p_mouse_position_change.p_mouse_change_y * -1.0f;
    transform_change.p_rotation.p_vertices[1] = (*game_information).p_controls.p_mouse_position_change.p_mouse_change_x;

    // draw everything
    RENDER__draw__world((*game_information).p_game_textures, (*game_information).p_world, (*game_information).p_chunks_shader_program, transform_change);

    // display window
    SDL_GL_SwapWindow((*game_information).p_graphics.p_window_context);

    // setup next frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

// checks for errors
BASIC__bt CONF2__check__error(GAME__information* game_information) {
    GLenum error_code;

    // quick thing to quiet compiler warning
    game_information = game_information;

    // check for opengl error
    error_code = glGetError();

    // check if error occured
    if (error_code != GL_NO_ERROR) {
        // print error to stdout
        printf("OpenGL Error: %lu\n", (u64)error_code);

        // return error occured
        return BASIC__bt__true;
    }

    // return no error occured
    return BASIC__bt__false;
}


#endif
