#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "rgb_map.h"
#include "lowrance.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main( int argc, char **argv )
{
    
}

int _unused( int argc, char **argv )
{
    FILE *sl_file;
    size_t n_read;
    sl_file_header_t header;
    sl_frame_header_t frame;
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
        n_read = fread( &header, 1, sizeof( sl_file_header_t ), sl_file );
        printf( "Read <%d> bytes\n", n_read );
        printf( "Format is <%d>, version is <%d>, blocksize is <%d>, file size is <%d>\n", header.format, header.version, header.blocksize, file_length );
        for (int i = 0; i < height; i++){
            n_read = fread( &frame, 1, sizeof( sl_frame_header_t ), sl_file );
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
            printf("Header size is %d, packet size is %d, total %d, blocksize report as %d\n", sizeof(sl_frame_header_t), frame.packet_size, sizeof(sl_frame_header_t) + frame.packet_size, frame.blocksize );
        }
        while(n_read > 0)
        {
            n_read = fread( &frame, 1, sizeof( sl_frame_header_t ), sl_file );
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
