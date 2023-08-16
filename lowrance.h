#pragma once
#include <stdint.h>
#include <stdio.h>

#define LOG_DEBUG printf
#define LOG_WARN  printf
#define LOG_ERROR printf
#define LOG_FATAL printf

typedef struct 
{
    uint16_t format;
    uint16_t version;
    uint16_t blocksize;
    uint16_t _unused;
} sl_file_header_t;

typedef struct 
{
    uint32_t frame_offset;
    uint32_t last_primary_channel_frame_offset;
    uint32_t last_secondary_channel_frame_offset;
    uint32_t last_downscan_channel_frame_offset;
    uint32_t last_left_sidescan_channel_frame_offset;
    uint32_t last_right_sidescan_channel_frame_offset;
    uint32_t last_composite_sidescan_channel_frame_offset;
    uint16_t blocksize; // size of current block in bytes
    uint16_t last_blocksize;
    uint16_t channel;
    uint16_t packet_size; // size of sounding data
    uint32_t frame_index; // starts at 0, match frames/blocks on different channels
    float upper_limit;
    float lower_limit;
    uint16_t _unused;
    uint8_t frequency;
    uint8_t _unknown[13];
    float water_depth;
    float keel_depth;
    uint8_t _unknown2[28];
    float speed_gps;
    float temperature_c;
    uint32_t enc_longitude;
    uint32_t enc_latitude;
    float speed_water;
    float course_over_ground_rad;
    float altitude_ft;
    float heading_rad;
    uint16_t flags;
    uint8_t _unknown3[6];
    uint32_t time; // unknown epoch
    // begin sounding data here
} sl_frame_header_t;

typedef struct 
{
    sl_frame_header_t header;
    uint8_t *sounder_data;
} sl_frame_t;

typedef struct 
{
    FILE *_file;
    uint8_t _file_name[64];
    size_t file_size;
    sl_file_header_t file_header;
} sl_file_t;

typedef enum
{
    SL_CHANNEL_PRIMARY   = 0,
    SL_CHANNEL_SECONDARY = 1,
    SL_CHANNEL_DSI       = 2,
    SL_CHANNEL_LEFT      = 3,
    SL_CHANNEL_RIGHT     = 4,
    SL_CHANNEL_COMPOSIT  = 5,
    SL_CHANNEL_INVALID
} sl_channel_t;

typedef enum
{
    SL_FREQ_200KHZ     = 0,
    SL_FREQ_50KHZ      = 1,
    SL_FREQ_83KHZ      = 2,
    SL_FREQ_455KHZ     = 3,
    SL_FREQ_800KHZ     = 4,
    SL_FREQ_38KHZ      = 5,
    SL_FREQ_28KHZ      = 6,
    SL_FREQ_130_210KHZ = 7,
    SL_FREQ_90_150KHZ  = 8,
    SL_FREQ_40_60KHZ   = 9,
    SL_FREQ_25_45KHZ   = 10,
    SL_FREQ_INVALID
} sl_frequency_t;


int sl_file_open( sl_file_t *file, uint8_t *file_name );
void sl_file_free( sl_file_t *file );
int sl_frame_get_next( sl_file_t *file, sl_frame_t *frame );
void sl_frame_print(const sl_frame_header_t *frame);
void sl_frame_free( sl_frame_t *frame );
