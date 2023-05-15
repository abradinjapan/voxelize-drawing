#ifndef VOXELIZE__render
#define VOXELIZE__render

/* Include */
// game internals
#include "shaders.h"
#include "textures.h"
#include "skin.h"
#include "../simulation/world.h"

// drawing info
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <math.h>

/* Define */
// vertex size information
typedef f32 RENDER__axis;
typedef RENDER__axis RENDER__x;
typedef RENDER__axis RENDER__y;
typedef RENDER__axis RENDER__z;

// chunk vbo data
typedef u8 RENDER__compact_axis;
typedef RENDER__compact_axis RENDER__compact_x; // chunk relative vertex offset
typedef RENDER__compact_axis RENDER__compact_y; // chunk relative vertex offset
typedef RENDER__compact_axis RENDER__compact_z; // chunk relative vertex offset
typedef u8 RENDER__compact_texture_xy; // texture sub indices
typedef TEX__face_count RENDER__compact_texture_z; // texture number / 2d texture array texture selector

// chunk vbo organizers
typedef u32 RENDER__compact_vertex_half;
typedef u64 RENDER__compact_vertex;

// chunk vbo data indexing
typedef u32 RENDER__vertex_index; // index into chunk's vertex array
typedef RENDER__vertex_index RENDER__vertex_count; // vertex count

// chunk indexing
typedef WORLD__chunks_index RENDER__object_index;
typedef RENDER__object_index RENDER__object_count;
typedef RENDER__object_index RENDER__object_axis;
typedef RENDER__object_axis RENDER__object_x;
typedef RENDER__object_axis RENDER__object_y;
typedef RENDER__object_axis RENDER__object_z;

// chunk vertex statistics
#define RENDER__chunk_side_block_count WORLD__chunk_side_block_count
#define RENDER__chunk_side_vertex_count 129 // ((256 / 2) + 1) <- Because doing 0 - 256 would require 9 bits, the detail is cut in half to 0 - 128 || 129 || 0 - (16 * 8)
#define RENDER__chunk_slice_vertex_count (RENDER__chunk_side_vertex_count*RENDER__chunk_side_vertex_count)
#define RENDER__chunk_total_vertex_count (RENDER__chunk_slice_vertex_count*RENDER__chunk_side_vertex_count)
#define RENDER__chunk_total_vertex_size (RENDER__chunk_total_vertex_count*sizeof(RENDER__compact_vertex))
#define RENDER__vertices_per_block_strip ((RENDER__chunk_side_vertex_count-1)/RENDER__chunk_side_block_count) // the distance from one block to the next

// chunk draw type information
#define RENDER__chunk_draw_type GL_ARRAY_BUFFER

/* Graphical Positions */
// three point vertex
typedef struct RENDER__vertex {
    vec3 p_vertices;
} RENDER__vertex;

// create custom vertex
RENDER__vertex RENDER__create__vertex(RENDER__x x, RENDER__y y, RENDER__z z) {
    RENDER__vertex output;

    // setup output
    output.p_vertices[0] = x;
    output.p_vertices[1] = y;
    output.p_vertices[2] = z;

    return output;
}

// information necessary to display object
typedef struct RENDER__transform {
    RENDER__vertex p_position_offset;
    RENDER__vertex p_rotation;
} RENDER__transform;

// create custom transform
RENDER__transform RENDER__create__transform(RENDER__vertex position_offset, RENDER__vertex rotation) {
    RENDER__transform output;

    // setup output
    output.p_position_offset = position_offset;
    output.p_rotation = rotation;

    return output;
}

/* Rendering Temporaries */
// temporaries
typedef struct RENDER__temporaries {
    BASIC__buffer p_vertices;
} RENDER__temporaries;

// create temporaries
RENDER__temporaries RENDER__open__temporaries() {
    RENDER__temporaries output;

    // setup output
    output.p_vertices = BASIC__create__buffer(RENDER__chunk_total_vertex_size);

    return output;
}

// destroy temporaries
void RENDER__close__temporaries(RENDER__temporaries temporaries) {
    // free buffers
    BASIC__destroy__buffer(temporaries.p_vertices);

    return;
}

// clear temporaries
void RENDER__clear__temporaries(RENDER__temporaries temporaries) {
    for (u64 i = 0; i < RENDER__chunk_total_vertex_size; i++) {
        ((u8*)temporaries.p_vertices.p_address)[i] = 0;
    }
}

/* The Visible World */
// object type
typedef enum RENDER__ot {
    RENDER__ot__chunk_body,
    RENDER__ot__chunk_XY_surface,
    RENDER__ot__chunk_YZ_surface,
    RENDER__ot__chunk_XZ_surface,
} RENDER__ot;

// an object's vbo and size information
typedef struct RENDER__object_handle {
    RENDER__transform p_transform;
    RENDER__vertex_count p_vertex_count;
    GLuint p_vao;
    GLuint p_vbo;
} RENDER__object_handle;

// chunk handle addressing
typedef RENDER__object_handle* RENDER__object_handle_address;

// create custom chunk handle
RENDER__object_handle RENDER__create__object_handle(RENDER__transform transform, RENDER__vertex_count vertex_count, GLuint vao, GLuint vbo) {
    RENDER__object_handle output;

    // setup output
    output.p_transform = transform;
    output.p_vertex_count = vertex_count;
    output.p_vao = vao;
    output.p_vbo = vbo;

    return output;
}

// dimensions of a 3d object
typedef struct RENDER__dimension_size {
    RENDER__object_x p_width;
    RENDER__object_y p_height;
    RENDER__object_z p_depth;
} RENDER__dimension_size;

// create a dimension size
RENDER__dimension_size RENDER__create__dimension_size(RENDER__object_x width, RENDER__object_y height, RENDER__object_z depth) {
    RENDER__dimension_size output;

    // setup output
    output.p_width = width;
    output.p_height = height;
    output.p_depth = depth;
    
    return output;
}

// calculate a dimensions array size
RENDER__object_count RENDER__calculate__dimension_size_total_count(RENDER__dimension_size dimension_size) {
    return dimension_size.p_width * dimension_size.p_height * dimension_size.p_depth;
}

// everything rendered in the game
typedef struct RENDER__world {
    // opengl vbo handles
    BASIC__buffer p_handles;

    // total count of handles
    RENDER__object_count p_handle_total_count;

    // counts of each type of drawable object
    RENDER__object_count p_chunk_bodies_count;
    RENDER__object_count p_chunk_XY_surfaces_count;
    RENDER__object_count p_chunk_YZ_surfaces_count;
    RENDER__object_count p_chunk_XZ_surfaces_count;

    // dimensions
    RENDER__dimension_size p_chunk_bodies_dimension_size;
    RENDER__dimension_size p_chunk_XY_surfaces_dimension_size;
    RENDER__dimension_size p_chunk_YZ_surfaces_dimension_size;
    RENDER__dimension_size p_chunk_XZ_surfaces_dimension_size;
} RENDER__world;

// close the visible world
void RENDER__close__world(RENDER__world world) {
    // close handles
    for (RENDER__object_index i = 0; i < world.p_handle_total_count; i++) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[i].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[i].p_vao));
    }

    // close buffers
    BASIC__destroy__buffer(world.p_handles);

    return;
}

// open visible world
RENDER__world RENDER__open__world(WORLD__chunks chunks) {
    RENDER__world output;

    // setup dimemsions
    output.p_chunk_bodies_dimension_size = RENDER__create__dimension_size(chunks.p_width, chunks.p_height, chunks.p_depth);
    output.p_chunk_XY_surfaces_dimension_size = RENDER__create__dimension_size(chunks.p_width, chunks.p_height, chunks.p_depth - 1);
    output.p_chunk_YZ_surfaces_dimension_size = RENDER__create__dimension_size(chunks.p_width - 1, chunks.p_height, chunks.p_depth);
    output.p_chunk_XZ_surfaces_dimension_size = RENDER__create__dimension_size(chunks.p_width, chunks.p_height - 1, chunks.p_depth);

    // setup counts
    output.p_chunk_bodies_count = (RENDER__object_count)RENDER__calculate__dimension_size_total_count(output.p_chunk_bodies_dimension_size);
    output.p_chunk_XY_surfaces_count = (RENDER__object_count)RENDER__calculate__dimension_size_total_count(output.p_chunk_XY_surfaces_dimension_size);
    output.p_chunk_YZ_surfaces_count = (RENDER__object_count)RENDER__calculate__dimension_size_total_count(output.p_chunk_YZ_surfaces_dimension_size);
    output.p_chunk_XZ_surfaces_count = (RENDER__object_count)RENDER__calculate__dimension_size_total_count(output.p_chunk_XZ_surfaces_dimension_size);

    // setup total count
    output.p_handle_total_count = output.p_chunk_bodies_count + output.p_chunk_XY_surfaces_count + output.p_chunk_YZ_surfaces_count + output.p_chunk_XZ_surfaces_count;

    // allocate chunk handles
    output.p_handles = BASIC__create__buffer(output.p_handle_total_count * sizeof(RENDER__object_handle));

    // setup chunk body handles
    for (RENDER__object_index i = 0; i < output.p_handle_total_count; i++) {
        // create blank handle
        ((RENDER__object_handle*)output.p_handles.p_address)[i] = RENDER__create__object_handle(RENDER__create__transform(RENDER__create__vertex(0.0f, 0.0f, 0.0f), RENDER__create__vertex(0.0f, 0.0f, 0.0f)), 0, 0, 0);
    }

    return output;
}

// calculate the offset of handle types in the handles array
RENDER__object_index RENDER__calculate__handle_index(RENDER__world world, RENDER__ot object_type, RENDER__object_x object_x, RENDER__object_y object_y, RENDER__object_z object_z) {
    // determing type & calculate offset accordingly
    switch (object_type) {
    case RENDER__ot__chunk_body:
        return 0 + (world.p_chunk_bodies_dimension_size.p_height * world.p_chunk_bodies_dimension_size.p_width * object_z) + (world.p_chunk_bodies_dimension_size.p_width * object_y) + object_x;
    case RENDER__ot__chunk_XY_surface:
        return world.p_chunk_bodies_count + (world.p_chunk_XY_surfaces_dimension_size.p_height * world.p_chunk_XY_surfaces_dimension_size.p_width * object_z) + (world.p_chunk_XY_surfaces_dimension_size.p_width * object_y) + object_x;
    case RENDER__ot__chunk_YZ_surface:
        return world.p_chunk_bodies_count + world.p_chunk_XY_surfaces_count + (world.p_chunk_YZ_surfaces_dimension_size.p_height * world.p_chunk_YZ_surfaces_dimension_size.p_width * object_z) + (world.p_chunk_YZ_surfaces_dimension_size.p_width * object_y) + object_x;
    case RENDER__ot__chunk_XZ_surface:
        return world.p_chunk_bodies_count + world.p_chunk_XY_surfaces_count + world.p_chunk_YZ_surfaces_count + (world.p_chunk_XZ_surfaces_dimension_size.p_height * world.p_chunk_XZ_surfaces_dimension_size.p_width * object_z) + (world.p_chunk_XZ_surfaces_dimension_size.p_width * object_y) + object_x;
    default:
        return world.p_handle_total_count;
    }
}

/* Render The World */
// assemble one vertex
RENDER__compact_vertex RENDER__render__one_vertex(RENDER__compact_x x, RENDER__compact_y y, RENDER__compact_z z, RENDER__compact_texture_xy texture_x, RENDER__compact_texture_xy texture_y, RENDER__compact_texture_z texture_z) {
    RENDER__compact_vertex output;

    // null initialize output
    output = 0;

    // setup reserved space & texture z
    output <<= 16; // split into 16 twice because of intel bitshifting limitations
    output <<= 16;
    output += (RENDER__compact_vertex)texture_z;

    // setup texture x and texture y
    output <<= 8;
    output += (RENDER__compact_vertex)((texture_y << 4) + texture_x);

    // setup texture z
    output <<= 8;
    output += (RENDER__compact_vertex)z;
    
    // setup texture y
    output <<= 8;
    output += (RENDER__compact_vertex)y;
    
    // setup texture x
    output <<= 8;
    output += (RENDER__compact_vertex)x;
    
    return output;
}

// render vertex and setup to render next vertex
void RENDER__render_and_next__compact_vertex(RENDER__vertex_index* index, RENDER__compact_vertex* output, RENDER__compact_vertex vertex) {
    // write data
    output[*index] = vertex;

    // advance index
    *index += 1;

    return;
}

// draw left & right face
RENDER__vertex_index RENDER__render__X_face(RENDER__temporaries temps, RENDER__vertex_index vertex_index, RENDER__x x, RENDER__y y, RENDER__z z, RENDER__compact_texture_z texture_z) {
    RENDER__compact_texture_xy texture_min = 0;
    RENDER__compact_texture_xy texture_max = 15;
    RENDER__compact_axis offset_size = RENDER__vertices_per_block_strip;

    // render first triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z, texture_min, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z + offset_size, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z, texture_max, texture_min, texture_z));

    // render second triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z + offset_size, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z, texture_max, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z + offset_size, texture_max, texture_max, texture_z));

    return vertex_index;
}

// draw top & bottom face
RENDER__vertex_index RENDER__render__Y_face(RENDER__temporaries temps, RENDER__vertex_index vertex_index, RENDER__x x, RENDER__y y, RENDER__z z, RENDER__compact_texture_z texture_z) {
    RENDER__compact_texture_xy texture_min = 0;
    RENDER__compact_texture_xy texture_max = 15;
    RENDER__compact_axis offset_size = RENDER__vertices_per_block_strip;

    // render first triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z, texture_min, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z + offset_size, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z, texture_max, texture_min, texture_z));

    // render second triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z, texture_max, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z + offset_size, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z + offset_size, texture_max, texture_max, texture_z));

    return vertex_index;
}

// draw back & front face
RENDER__vertex_index RENDER__render__Z_face(RENDER__temporaries temps, RENDER__vertex_index vertex_index, RENDER__x x, RENDER__y y, RENDER__z z, RENDER__compact_texture_z texture_z) {
    RENDER__compact_texture_xy texture_min = 0;
    RENDER__compact_texture_xy texture_max = 15;
    RENDER__compact_axis offset_size = RENDER__vertices_per_block_strip;

    // render first triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y, z, texture_min, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z, texture_max, texture_min, texture_z));

    // render second triangle
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x, y + offset_size, z, texture_min, texture_max, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y, z, texture_max, texture_min, texture_z));
    RENDER__render_and_next__compact_vertex(&vertex_index, temps.p_vertices.p_address, RENDER__render__one_vertex(x + offset_size, y + offset_size, z, texture_max, texture_max, texture_z));

    return vertex_index;
}

// create the vertices for the chunk insides and send it to opengl
void RENDER__render__chunk_body(SKIN__skins skins, WORLD__chunk_address chunk_address, RENDER__object_index handle_index, RENDER__world world, RENDER__temporaries temps) {
    RENDER__vertex_index vertex_index;
    WORLD__block_ID block_center_type;
    WORLD__block_ID block_side_type;
    RENDER__x chunk_x;
    RENDER__y chunk_y;
    RENDER__z chunk_z;
    RENDER__axis offset_size = RENDER__vertices_per_block_strip;

    // setup chunk data index
    vertex_index = 0;

    // for each chunk slice
    for (WORLD__chunk_axis_index chunk_slice = 0; chunk_slice < RENDER__chunk_side_block_count; chunk_slice++) { // Z
        // for each chunk strip
        for (WORLD__chunk_axis_index chunk_strip = 0; chunk_strip < RENDER__chunk_side_block_count; chunk_strip++) { // Y
            // for each block
            for (WORLD__chunk_axis_index chunk_block = 0; chunk_block < RENDER__chunk_side_block_count; chunk_block++) { // X
                // get center block type
                block_center_type = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(chunk_address, WORLD__calculate__block_index(chunk_block, chunk_strip, chunk_slice)));

                // try draw left face
                // check if face is on outside (if so, do not draw)
                if (chunk_block > 0) {
                    // get block on left side
                    block_side_type = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(chunk_address, WORLD__calculate__block_index(chunk_block - 1, chunk_strip, chunk_slice)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__X_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__left]);
                    }
                }

                // try draw right face
                // check if face is on outside (if so, do not draw)
                if (chunk_block < RENDER__chunk_side_block_count - 1) {
                    // get block on right side
                    block_side_type = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(chunk_address, WORLD__calculate__block_index(chunk_block + 1, chunk_strip, chunk_slice)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block + 1) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__X_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__right]);
                    }
                }

                // try draw top face
                // check if face is on outside (if so, do not draw)
                if (chunk_strip < RENDER__chunk_side_block_count - 1) {
                    // get block on top side
                    block_side_type = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(chunk_address, WORLD__calculate__block_index(chunk_block, chunk_strip + 1, chunk_slice)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip + 1) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__Y_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__top]);
                    }
                }

                // try draw bottom face
                // check if face is on outside (if so, do not draw)
                if (chunk_strip > 0) {
                    // get block on bottom side
                    block_side_type = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(chunk_address, WORLD__calculate__block_index(chunk_block, chunk_strip - 1, chunk_slice)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__Y_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__bottom]);
                    }
                }
                
                // try draw back face
                // check if face is on outside (if so, do not draw)
                if (chunk_slice < RENDER__chunk_side_block_count - 1) {
                    // get block on back side
                    block_side_type = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(chunk_address, WORLD__calculate__block_index(chunk_block, chunk_strip, chunk_slice + 1)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice + 1) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__Z_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__back]);
                    }
                }

                // try draw front face
                // check if face is on outside (if so, do not draw)
                if (chunk_slice > 0) {
                    // get block on front side
                    block_side_type = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(chunk_address, WORLD__calculate__block_index(chunk_block, chunk_strip, chunk_slice - 1)));

                    // check if face would be hidden (if so, do not draw)
                    if (SKIN__get__skin__block(skins, block_side_type).p_draw_type != SKIN__bdt__draw_only_one_side) {
                        // get vertex values
                        chunk_x = ((float)chunk_block) * offset_size;
                        chunk_y = ((float)chunk_strip) * offset_size;
                        chunk_z = ((float)chunk_slice) * offset_size;

                        // draw face
                        vertex_index = RENDER__render__Z_face(temps, vertex_index, chunk_x, chunk_y, chunk_z, SKIN__get__skin__block(skins, block_center_type).p_faces[SKIN__bst__front]);
                    }
                }
            }
        }
    }

    // update handle
    ((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vertex_count = vertex_index;

    // delete if necessary
    if (((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao != 0) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao));
    }

    // initialize handle
    glGenVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao));
    glBindVertexArray(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao);
    glGenBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo));
    glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo);

    // setup vertex attribute array configuration
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)sizeof(RENDER__compact_vertex_half));
    glEnableVertexAttribArray(1);

    // send data
    glBufferData(RENDER__chunk_draw_type, vertex_index * sizeof(RENDER__compact_vertex), (RENDER__compact_vertex*)temps.p_vertices.p_address, GL_STATIC_DRAW);

    return;
}

// check if a side can be rendered
BASIC__bt RENDER__calculate__chunk_surface_face_is_renderable(SKIN__bdt center_block_draw_type, SKIN__bdt outside_block_draw_type) {
    return (BASIC__bt)(center_block_draw_type == SKIN__bdt__draw_all_sides || (center_block_draw_type > SKIN__bdt__dont_draw && outside_block_draw_type == SKIN__bdt__dont_draw));
}

// render a chunk XY surface
void RENDER__render__chunk_XY_surface(SKIN__skins skins, WORLD__chunks chunks, RENDER__object_index handle_index, WORLD__chunks_index center_chunks_index, WORLD__chunks_index outside_chunks_index, RENDER__world world, RENDER__temporaries temps) {
    WORLD__chunk_address center_chunk_address;
    WORLD__chunk_address outside_chunk_address;
    WORLD__block_ID center_block_ID;
    WORLD__block_ID outside_block_ID;
    RENDER__x vertex_x;
    RENDER__y vertex_y;
    RENDER__z vertex_z;
    RENDER__vertex_index vertex_index;
    RENDER__axis offset_size = RENDER__vertices_per_block_strip;

    // setup chunk pointer
    center_chunk_address = WORLD__get__chunk_pointer_in_chunks(chunks, center_chunks_index);
    outside_chunk_address = WORLD__get__chunk_pointer_in_chunks(chunks, outside_chunks_index);

    // setup chunk data index
    vertex_index = 0;

    // for each x axis
    for (RENDER__object_index block_x = 0; block_x < RENDER__chunk_side_block_count; block_x++) {
        // for each y axis
        for (RENDER__object_index block_y = 0; block_y < RENDER__chunk_side_block_count; block_y++) {
            // get block data
            center_block_ID = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(center_chunk_address, WORLD__calculate__block_index(block_x, block_y, WORLD__chunk_side_block_count - 1)));
            outside_block_ID = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(outside_chunk_address, WORLD__calculate__block_index(block_x, block_y, 0)));
            
            // if the first side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, center_block_ID).p_draw_type, SKIN__get__skin__block(skins, outside_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = ((float)block_x) * offset_size;
                vertex_y = ((float)block_y) * offset_size;
                vertex_z = 0;
                
                // draw face
                vertex_index = RENDER__render__Z_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, center_block_ID).p_faces[SKIN__bst__front]);
            }

            // if the second side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, outside_block_ID).p_draw_type, SKIN__get__skin__block(skins, center_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = ((float)block_x) * offset_size;
                vertex_y = ((float)block_y) * offset_size;
                vertex_z = 0;
                
                // draw face
                vertex_index = RENDER__render__Z_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, outside_block_ID).p_faces[SKIN__bst__back]);
            }
        }
    }

    // update handle
    ((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vertex_count = vertex_index;

    // delete if necessary
    if (((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao != 0) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao));
    }

    // initialize handle
    glGenVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao));
    glBindVertexArray(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao);
    glGenBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo));
    glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo);

    // setup vertex attribute array configuration
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)sizeof(RENDER__compact_vertex_half));
    glEnableVertexAttribArray(1);

    // send data
    glBufferData(RENDER__chunk_draw_type, vertex_index * sizeof(RENDER__compact_vertex), (RENDER__compact_vertex*)temps.p_vertices.p_address, GL_STATIC_DRAW);

    return;
}

// render a chunk YZ surface
void RENDER__render__chunk_YZ_surface(SKIN__skins skins, WORLD__chunks chunks, RENDER__object_index handle_index, WORLD__chunks_index center_chunks_index, WORLD__chunks_index outside_chunks_index, RENDER__world world, RENDER__temporaries temps) {
    WORLD__chunk_address center_chunk_address;
    WORLD__chunk_address outside_chunk_address;
    WORLD__block_ID center_block_ID;
    WORLD__block_ID outside_block_ID;
    RENDER__x vertex_x;
    RENDER__y vertex_y;
    RENDER__z vertex_z;
    RENDER__vertex_index vertex_index;
    RENDER__axis offset_size = RENDER__vertices_per_block_strip;

    // setup chunk pointer
    center_chunk_address = WORLD__get__chunk_pointer_in_chunks(chunks, center_chunks_index);
    outside_chunk_address = WORLD__get__chunk_pointer_in_chunks(chunks, outside_chunks_index);

    // setup chunk data index
    vertex_index = 0;

    // for each x axis
    for (RENDER__object_index block_y = 0; block_y < RENDER__chunk_side_block_count; block_y++) {
        // for each y axis
        for (RENDER__object_index block_z = 0; block_z < RENDER__chunk_side_block_count; block_z++) {
            // get block data
            center_block_ID = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(center_chunk_address, WORLD__calculate__block_index(WORLD__chunk_side_block_count - 1, block_y, block_z)));
            outside_block_ID = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(outside_chunk_address, WORLD__calculate__block_index(0, block_y, block_z)));
        
            // if the first side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, center_block_ID).p_draw_type, SKIN__get__skin__block(skins, outside_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = 0;
                vertex_y = ((float)block_y) * offset_size;
                vertex_z = ((float)block_z) * offset_size;
                
                // draw face
                vertex_index = RENDER__render__X_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, center_block_ID).p_faces[SKIN__bst__left]);
            }

            // if the second side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, outside_block_ID).p_draw_type, SKIN__get__skin__block(skins, center_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = 0;
                vertex_y = ((float)block_y) * offset_size;
                vertex_z = ((float)block_z) * offset_size;
                
                // draw face
                vertex_index = RENDER__render__X_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, outside_block_ID).p_faces[SKIN__bst__right]);
            }
        }
    }

    // update handle
    ((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vertex_count = vertex_index;

    // delete if necessary
    if (((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao != 0) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao));
    }

    // initialize handle
    glGenVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao));
    glBindVertexArray(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao);
    glGenBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo));
    glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo);

    // setup vertex attribute array configuration
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)sizeof(RENDER__compact_vertex_half));
    glEnableVertexAttribArray(1);

    // send data
    glBufferData(RENDER__chunk_draw_type, vertex_index * sizeof(RENDER__compact_vertex), (RENDER__compact_vertex*)temps.p_vertices.p_address, GL_STATIC_DRAW);

    return;
}

// render a chunk XZ surface
void RENDER__render__chunk_XZ_surface(SKIN__skins skins, WORLD__chunks chunks, RENDER__object_index handle_index, WORLD__chunks_index center_chunks_index, WORLD__chunks_index outside_chunks_index, RENDER__world world, RENDER__temporaries temps) {
    WORLD__chunk_address center_chunk_address;
    WORLD__chunk_address outside_chunk_address;
    WORLD__block_ID center_block_ID;
    WORLD__block_ID outside_block_ID;
    RENDER__x vertex_x;
    RENDER__y vertex_y;
    RENDER__z vertex_z;
    RENDER__vertex_index vertex_index;
    RENDER__axis offset_size = RENDER__vertices_per_block_strip;

    // setup chunk pointer
    center_chunk_address = WORLD__get__chunk_pointer_in_chunks(chunks, center_chunks_index);
    outside_chunk_address = WORLD__get__chunk_pointer_in_chunks(chunks, outside_chunks_index);

    // setup chunk data index
    vertex_index = 0;

    // for each x axis
    for (RENDER__object_index block_x = 0; block_x < RENDER__chunk_side_block_count; block_x++) {
        // for each y axis
        for (RENDER__object_index block_z = 0; block_z < RENDER__chunk_side_block_count; block_z++) {
            // get block data
            center_block_ID = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(center_chunk_address, WORLD__calculate__block_index(block_x, WORLD__chunk_side_block_count - 1, block_z)));
            outside_block_ID = WORLD__get__block_ID_from_block_data(WORLD__get__block_data_from_chunk_address(outside_chunk_address, WORLD__calculate__block_index(block_x, 0, block_z)));
        
            // if the first side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, center_block_ID).p_draw_type, SKIN__get__skin__block(skins, outside_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = ((float)block_x) * offset_size;
                vertex_y = 0;
                vertex_z = ((float)block_z) * offset_size;
                
                // draw face
                vertex_index = RENDER__render__Y_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, center_block_ID).p_faces[SKIN__bst__top]);
            }

            // if the second side should be drawn
            if (RENDER__calculate__chunk_surface_face_is_renderable(SKIN__get__skin__block(skins, outside_block_ID).p_draw_type, SKIN__get__skin__block(skins, center_block_ID).p_draw_type) == BASIC__bt__true) {
                // get vertex values
                vertex_x = ((float)block_x) * offset_size;
                vertex_y = 0;
                vertex_z = ((float)block_z) * offset_size;
                
                // draw face
                vertex_index = RENDER__render__Y_face(temps, vertex_index, vertex_x, vertex_y, vertex_z, SKIN__get__skin__block(skins, outside_block_ID).p_faces[SKIN__bst__bottom]);
            }
        }
    }

    // update handle
    ((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vertex_count = vertex_index;

    // delete if necessary
    if (((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao != 0) {
        glDeleteBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo));
        glDeleteVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao));
    }

    // initialize handle
    glGenVertexArrays(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao));
    glBindVertexArray(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vao);
    glGenBuffers(1, &(((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo));
    glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_handles.p_address)[handle_index].p_vbo);

    // setup vertex attribute array configuration
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RENDER__compact_vertex), (void*)sizeof(RENDER__compact_vertex_half));
    glEnableVertexAttribArray(1);

    // send data
    glBufferData(RENDER__chunk_draw_type, vertex_index * sizeof(RENDER__compact_vertex), (RENDER__compact_vertex*)temps.p_vertices.p_address, GL_STATIC_DRAW);

    return;
}

// render everything in the world
void RENDER__render__world(SKIN__skins skins, WORLD__chunks chunks, RENDER__world world, RENDER__temporaries temps) {
    // render each chunk body
    for (RENDER__object_index chunks_x = 0; chunks_x < world.p_chunk_bodies_dimension_size.p_width; chunks_x++) {
        for (RENDER__object_index chunks_y = 0; chunks_y < world.p_chunk_bodies_dimension_size.p_height; chunks_y++) {
            for (RENDER__object_index chunks_z = 0; chunks_z < world.p_chunk_bodies_dimension_size.p_depth; chunks_z++) {
                // render one chunk
                RENDER__render__chunk_body(skins, WORLD__get__chunk_pointer_in_chunks(chunks, WORLD__calculate__chunks_index(chunks, chunks_x, chunks_y, chunks_z)), (RENDER__object_index)WORLD__calculate__chunks_index(chunks, chunks_x, chunks_y, chunks_z), world, temps);

                // setup chunk offset
                ((RENDER__object_handle*)world.p_handles.p_address)[RENDER__calculate__handle_index(world, RENDER__ot__chunk_body, chunks_x, chunks_y, chunks_z)].p_transform = RENDER__create__transform(RENDER__create__vertex((float)(chunks_x) * ((float)RENDER__chunk_side_vertex_count - 1), (float)(chunks_y) * ((float)RENDER__chunk_side_vertex_count - 1), (float)(chunks_z) * ((float)RENDER__chunk_side_vertex_count - 1)), RENDER__create__vertex(0.0f, 0.0f, 0.0f));
            }
        }
    }

    // render each XY surface
    for (RENDER__object_index surfaces_x = 0; surfaces_x < world.p_chunk_XY_surfaces_dimension_size.p_width; surfaces_x++) {
        for (RENDER__object_index surfaces_y = 0; surfaces_y < world.p_chunk_XY_surfaces_dimension_size.p_height; surfaces_y++) {
            for (RENDER__object_index surfaces_z = 0; surfaces_z < world.p_chunk_XY_surfaces_dimension_size.p_depth; surfaces_z++) {
                // render one chunk
                RENDER__render__chunk_XY_surface(skins, chunks, RENDER__calculate__handle_index(world, RENDER__ot__chunk_XY_surface, surfaces_x, surfaces_y, surfaces_z), WORLD__calculate__chunks_index(chunks, (WORLD__chunks_x)surfaces_x, (WORLD__chunks_y)surfaces_y, (WORLD__chunks_z)surfaces_z), WORLD__calculate__chunks_index(chunks, (WORLD__chunks_x)surfaces_x, (WORLD__chunks_y)surfaces_y, (WORLD__chunks_z)surfaces_z + 1), world, temps);

                // setup chunk offset
                ((RENDER__object_handle*)world.p_handles.p_address)[RENDER__calculate__handle_index(world, RENDER__ot__chunk_XY_surface, surfaces_x, surfaces_y, surfaces_z)].p_transform = RENDER__create__transform(RENDER__create__vertex((float)(surfaces_x) * ((float)RENDER__chunk_side_vertex_count - 1), (float)(surfaces_y) * ((float)RENDER__chunk_side_vertex_count - 1), (float)(surfaces_z + 1) * ((float)RENDER__chunk_side_vertex_count - 1)), RENDER__create__vertex(0.0f, 0.0f, 0.0f));
            }
        }
    }

    // render each YZ surface
    for (RENDER__object_index surfaces_x = 0; surfaces_x < world.p_chunk_YZ_surfaces_dimension_size.p_width; surfaces_x++) {
        for (RENDER__object_index surfaces_y = 0; surfaces_y < world.p_chunk_YZ_surfaces_dimension_size.p_height; surfaces_y++) {
            for (RENDER__object_index surfaces_z = 0; surfaces_z < world.p_chunk_YZ_surfaces_dimension_size.p_depth; surfaces_z++) {
                // render one chunk
                RENDER__render__chunk_YZ_surface(skins, chunks, RENDER__calculate__handle_index(world, RENDER__ot__chunk_YZ_surface, surfaces_x, surfaces_y, surfaces_z), WORLD__calculate__chunks_index(chunks, (WORLD__chunks_x)surfaces_x, (WORLD__chunks_y)surfaces_y, (WORLD__chunks_z)surfaces_z), WORLD__calculate__chunks_index(chunks, (WORLD__chunks_x)surfaces_x + 1, (WORLD__chunks_y)surfaces_y, (WORLD__chunks_z)surfaces_z), world, temps);

                // setup chunk offset
                ((RENDER__object_handle*)world.p_handles.p_address)[RENDER__calculate__handle_index(world, RENDER__ot__chunk_YZ_surface, surfaces_x, surfaces_y, surfaces_z)].p_transform = RENDER__create__transform(RENDER__create__vertex((float)(surfaces_x + 1) * ((float)RENDER__chunk_side_vertex_count - 1), (float)(surfaces_y) * ((float)RENDER__chunk_side_vertex_count - 1), (float)(surfaces_z) * ((float)RENDER__chunk_side_vertex_count - 1)), RENDER__create__vertex(0.0f, 0.0f, 0.0f));
            }
        }
    }

    // render each XZ surface
    for (RENDER__object_index surfaces_x = 0; surfaces_x < world.p_chunk_XZ_surfaces_dimension_size.p_width; surfaces_x++) {
        for (RENDER__object_index surfaces_y = 0; surfaces_y < world.p_chunk_XZ_surfaces_dimension_size.p_height; surfaces_y++) {
            for (RENDER__object_index surfaces_z = 0; surfaces_z < world.p_chunk_XZ_surfaces_dimension_size.p_depth; surfaces_z++) {
                // render one chunk
                RENDER__render__chunk_XZ_surface(skins, chunks, RENDER__calculate__handle_index(world, RENDER__ot__chunk_XZ_surface, surfaces_x, surfaces_y, surfaces_z), WORLD__calculate__chunks_index(chunks, (WORLD__chunks_x)surfaces_x, (WORLD__chunks_y)surfaces_y, (WORLD__chunks_z)surfaces_z), WORLD__calculate__chunks_index(chunks, (WORLD__chunks_x)surfaces_x, (WORLD__chunks_y)surfaces_y + 1, (WORLD__chunks_z)surfaces_z), world, temps);

                // setup chunk offset
                ((RENDER__object_handle*)world.p_handles.p_address)[RENDER__calculate__handle_index(world, RENDER__ot__chunk_XZ_surface, surfaces_x, surfaces_y, surfaces_z)].p_transform = RENDER__create__transform(RENDER__create__vertex((float)(surfaces_x) * ((float)RENDER__chunk_side_vertex_count - 1), (float)(surfaces_y + 1) * ((float)RENDER__chunk_side_vertex_count - 1), (float)(surfaces_z) * ((float)RENDER__chunk_side_vertex_count - 1)), RENDER__create__vertex(0.0f, 0.0f, 0.0f));
            }
        }
    }

    return;
}

/* Draw World */
// add position offsets and rotations
RENDER__transform RENDER__calculate__add_object_transforms(RENDER__transform a, RENDER__transform b) {
    RENDER__transform output;

    // setup output
    output.p_position_offset.p_vertices[0] = a.p_position_offset.p_vertices[0] + b.p_position_offset.p_vertices[0];
    output.p_position_offset.p_vertices[1] = a.p_position_offset.p_vertices[1] + b.p_position_offset.p_vertices[1];
    output.p_position_offset.p_vertices[2] = a.p_position_offset.p_vertices[2] + b.p_position_offset.p_vertices[2];
    output.p_rotation.p_vertices[0] = a.p_rotation.p_vertices[0] + b.p_rotation.p_vertices[0];
    output.p_rotation.p_vertices[1] = a.p_rotation.p_vertices[1] + b.p_rotation.p_vertices[1];
    output.p_rotation.p_vertices[2] = a.p_rotation.p_vertices[2] + b.p_rotation.p_vertices[2];

    return output;
}

// send transform matrix to GPU
void RENDER__send__transform_matrix(GLuint program_ID, mat4* transform) {
    // send data
    glUniformMatrix4fv(glGetUniformLocation(program_ID, "GLOBAL_transform"), 1, GL_FALSE, (RENDER__axis*)transform);

    return;
}

// update object transform and render to opengl
void RENDER__calculate_and_send__game_object_transform_matrix(RENDER__object_handle_address handle_address, SHADER__program shader_program, RENDER__transform camera_transform) {
    mat4 final_transform;
    mat4 perspective;
    mat4 view;
    mat4 model;

    // calculation values
    vec3 camera_position = {0.0f, 0.0f, 0.0f};
    vec3 camera_target = {0.0f, 0.0f, 2.0f};
    vec3 camera_up = {0.0f, 1.0f, 0.0f};
    vec3 rotate_x = {1.0f, 0.0f, 0.0f};
    vec3 rotate_y = {0.0f, 1.0f, 0.0f};
    vec3 camera_position_independent_movement;
    f32 camera_yaw;
    f32 camera_pitch;

    // update world object transform information
    (*handle_address).p_transform = RENDER__calculate__add_object_transforms((*handle_address).p_transform, camera_transform);

    // setup temps
    camera_position_independent_movement[0] = (*handle_address).p_transform.p_position_offset.p_vertices[0];
    camera_position_independent_movement[1] = (*handle_address).p_transform.p_position_offset.p_vertices[1];
    camera_position_independent_movement[2] = (*handle_address).p_transform.p_position_offset.p_vertices[2];
    camera_yaw = glm_rad((*handle_address).p_transform.p_rotation.p_vertices[0]);
    camera_pitch = glm_rad((*handle_address).p_transform.p_rotation.p_vertices[1]);

    // setup perspective
    glm_perspective(glm_rad(45.0f), 720.0f / 480.0f, 0.01f, 1000.0f, perspective);

    // setup view
    glm_lookat(camera_position, camera_target, camera_up, view);

    // setup model
    glm_mat4_identity(model);
    glm_rotate_at(model, camera_position, camera_yaw, rotate_x);
    glm_rotate_at(model, camera_position, camera_pitch, rotate_y);
    glm_translate(model, camera_position_independent_movement);

    // create final transform
    glm_mat4_mul(perspective, view, final_transform);
    glm_mat4_mul(final_transform, model, final_transform);

    // send final transform to opengl
    RENDER__send__transform_matrix(shader_program.p_program_ID, &final_transform);

    return;
}

/*// update object transform and render to opengl
void RENDER__calculate_and_send__game_object_transform_matrix(RENDER__object_handle_address handle_address, SHADER__program shader_program, RENDER__transform camera_transform) {
    mat4 final_transform;
    mat4 perspective;
    mat4 view;
    mat4 model;

    // calculation values
    vec3 camera_position = {0.0f, 0.0f, 0.0f};
    vec3 camera_target = {0.0f, 0.0f, 2.0f};
    vec3 camera_up = {0.0f, 1.0f, 0.0f};
    vec3 rotate_x = {1.0f, 0.0f, 0.0f};
    vec3 rotate_y = {0.0f, 1.0f, 0.0f};
    vec3 rotate_z = {0.0f, 0.0f, 1.0f};
    vec3 camera_position_dependent_movement;
    vec3 camera_front;
    f32 camera_yaw;
    f32 camera_pitch;

    // update world object transform information
    (*handle_address).p_transform = RENDER__calculate__add_object_transforms((*handle_address).p_transform, camera_transform);

    // setup temps
    camera_position_dependent_movement[0] = (*handle_address).p_transform.p_position_offset.p_vertices[0];
    camera_position_dependent_movement[1] = (*handle_address).p_transform.p_position_offset.p_vertices[1];
    camera_position_dependent_movement[2] = (*handle_address).p_transform.p_position_offset.p_vertices[2];
    camera_yaw = glm_rad((*handle_address).p_transform.p_rotation.p_vertices[0]);
    camera_pitch = glm_rad((*handle_address).p_transform.p_rotation.p_vertices[1]);

    // do math on temps
    camera_front[0] = cos(glm_rad(camera_yaw)) * cos(glm_rad(camera_pitch));
    camera_front[1] = sin(glm_rad(camera_pitch));
    camera_front[2] = sin(glm_rad(camera_yaw)) * cos(glm_rad(camera_pitch));
    glm_vec3_normalize(camera_front);
    glm_vec3_cross(camera_front, camera_up, camera_front);
    glm_vec3_add(camera_front, camera_position, camera_target);

    // setup perspective
    glm_perspective(glm_rad(45.0f), 720.0f / 480.0f, 0.01f, 1000.0f, perspective);

    // setup view
    glm_lookat(camera_position, camera_target, camera_up, view);

    // setup model
    glm_mat4_identity(model);
    glm_translate(model, camera_position_dependent_movement);
    glm_rotate_at(model, camera_position, camera_yaw, rotate_x);
    glm_rotate_at(model, camera_position, camera_pitch, rotate_y);

    // create final transform
    glm_mat4_mul(perspective, view, final_transform);
    glm_mat4_mul(final_transform, model, final_transform);

    // send final transform to opengl
    RENDER__send__transform_matrix(shader_program.p_program_ID, &final_transform);

    return;
}*/

// draw everything
void RENDER__draw__world(TEX__game_textures game_textures, RENDER__world world, SHADER__program shader_program, RENDER__transform camera_transform) {// bind proper textures
    // bind proper textures
    TEX__bind__game_textures__specific(game_textures, TEX__gtt__block_faces, shader_program);

    // draw all block handles
    for (RENDER__object_index i = 0; i < world.p_chunk_bodies_count + world.p_chunk_XY_surfaces_count + world.p_chunk_YZ_surfaces_count + world.p_chunk_XZ_surfaces_count; i++) {
        // create and send matrix to opengl
        RENDER__calculate_and_send__game_object_transform_matrix(world.p_handles.p_address + (sizeof(RENDER__object_handle) * i), shader_program, camera_transform);

        // bind chunk
        glBindVertexArray(((RENDER__object_handle*)world.p_handles.p_address)[i].p_vao);
        glBindBuffer(RENDER__chunk_draw_type, ((RENDER__object_handle*)world.p_handles.p_address)[i].p_vbo);

        // draw chunk
        glDrawArrays(GL_TRIANGLES, 0, ((RENDER__object_handle*)world.p_handles.p_address)[i].p_vertex_count);
    }

    return;
}

#endif
