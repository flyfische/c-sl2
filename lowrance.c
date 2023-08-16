#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "rgb_map.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct 
{
    uint16_t format;
    uint16_t version;
    uint16_t blocksize;
    uint16_t _unused;
} sl_header_t;

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
} sl_frame_t;

void print_sl_frame(const sl_frame_t *frame) {
    printf("frame_offset: %u\n", frame->frame_offset);
    printf("last_primary_channel_frame_offset: %u\n", frame->last_primary_channel_frame_offset);
    printf("last_secondary_channel_frame_offset: %u\n", frame->last_secondary_channel_frame_offset);
    printf("last_downscan_channel_frame_offset: %u\n", frame->last_downscan_channel_frame_offset);
    printf("last_left_sidescan_channel_frame_offset: %u\n", frame->last_left_sidescan_channel_frame_offset);
    printf("last_right_sidescan_channel_frame_offset: %u\n", frame->last_right_sidescan_channel_frame_offset);
    printf("last_composite_sidescan_channel_frame_offset: %u\n", frame->last_composite_sidescan_channel_frame_offset);
    printf("blocksize: %u\n", frame->blocksize);
    printf("last_blocksize: %u\n", frame->last_blocksize);
    printf("channel: %u\n", frame->channel);
    printf("packet_size: %u\n", frame->packet_size);
    printf("frame_index: %u\n", frame->frame_index);
    printf("upper_limit: %f\n", frame->upper_limit);
    printf("lower_limit: %f\n", frame->lower_limit);
    printf("_unused: %u\n", frame->_unused);
    printf("frequency: %u\n", frame->frequency);
    printf("water_depth: %f\n", frame->water_depth);
    printf("keel_depth: %f\n", frame->keel_depth);
    printf("speed_gps: %f\n", frame->speed_gps);
    printf("temperature_c: %f\n", frame->temperature_c);
    printf("enc_longitude: %u\n", frame->enc_longitude);
    printf("enc_latitude: %u\n", frame->enc_latitude);
    printf("speed_water: %f\n", frame->speed_water);
    printf("course_over_ground_rad: %f\n", frame->course_over_ground_rad);
    printf("altitude_ft: %f\n", frame->altitude_ft);
    printf("heading_rad: %f\n", frame->heading_rad);
    printf("flags: %u\n", frame->flags);
    printf("time: %u\n", frame->time);
    // ... (print sounding data)
}

int main( int argc, char **argv )
{
    FILE *sl_file;
    size_t n_read;
    sl_header_t header;
    sl_frame_t frame;
    uint8_t sounder_data[4096];
    uint8_t buffer[32];
    size_t sounder_len;
    int width = 1000;
    int height = 5500;
    sl_file = fopen("sonarfresh.sl2", "r");
    uint8_t *image_data;
    int channels = 3;
    uint8_t rgb_data[3];
    size_t data_ptr = 0;
    size_t file_length;
    size_t frame_counter = 0;
    image_data = (uint8_t*)malloc(height * width * channels * sizeof(uint8_t));
    printf("image_data size is %d\n", height * width * channels * sizeof(uint8_t) );
    if ( NULL == image_data )
    {
        printf("ERROR: unable to allocate memory");
        return 1;
    }
    if ( sl_file )
    {
        fseek( sl_file, 0, SEEK_END );
        file_length = ftell( sl_file );
        rewind( sl_file );
        n_read = fread( &header, 1, sizeof( sl_header_t ), sl_file );
        printf( "Read <%d> bytes\n", n_read );
        printf( "Format is <%d>, version is <%d>, blocksize is <%d>, file size is <%d>\n", header.format, header.version, header.blocksize, file_length );
        for (int i = 0; i < height; i++){
            n_read = fread( &frame, 1, sizeof( sl_frame_t ), sl_file );
            //printf( "Read <%d> bytes, sounder len is <%d>, water depth is <%f>\n", n_read, frame.packet_size, frame.water_depth );
            //printf( "Channel is <%d>\n", frame.channel );
            //print_sl_frame( &frame );
            sounder_len = fread( sounder_data, 1, frame.packet_size, sl_file );
            if ( frame.channel != 2 )
            {
                i -= 1;
                continue;
            }
            //printf("Water depth is %f\n", frame.water_depth);
            //memcpy( image_data + data_ptr,sounder_data,width);
            for(int j = 0; j < width; j++)
            {
                rgb_data[0] = rgb_map[sounder_data[j]][0];
                rgb_data[1] = rgb_map[sounder_data[j]][1];
                rgb_data[2] = rgb_map[sounder_data[j]][2];
                memcpy(image_data + data_ptr, rgb_data, 3);
                data_ptr += 3;
            }
            //data_ptr += width;
            frame_counter++;
            //printf("data_ptr is <%d>\n", data_ptr);
        }
        while(n_read > 0)
        {
            n_read = fread( &frame, 1, sizeof( sl_frame_t ), sl_file );
            n_read = fread( sounder_data, 1, frame.packet_size, sl_file );
            if (frame.channel == 2)
            {
                frame_counter++;

            }
        }
    }
    stbi_flip_vertically_on_write( 1 );
    stbi_write_jpg("output.jpg", width, height, channels, image_data,100);
    system("convert output.jpg -rotate 90 output.jpg");
    printf("Found %d total frames\n", frame_counter);
    free( image_data );
    fclose( sl_file );
    return 0;
}
