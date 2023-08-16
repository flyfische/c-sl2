#include "lowrance.h"
#include <string.h>
#include <stdlib.h>

void sl_frame_print(const sl_frame_header_t *frame) {
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

void sl_frame_free( sl_frame_t *frame )
{
    if ( NULL != frame->sounder_data )
    {
        free( frame->sounder_data );
    }
}
void sl_file_free( sl_file_t *file )
{
    if ( NULL != file->_file )
    {
        fclose( file->_file );
    }
}

int sl_file_open( sl_file_t *file, uint8_t *file_name )
{
    size_t n_read;
    file->_file = fopen( file_name, "r" );
    if ( NULL == file->_file )
    {
        LOG_FATAL( "Error opening file <%s>\n", file_name );
        return 1;
    }
    strncpy( file->_file_name, file_name, sizeof( file->_file_name ) - 1 );
    fseek( file->_file, 0, SEEK_END );
    file->file_size = ftell( file->_file );
    rewind( file->_file );
    LOG_DEBUG( "Opened file <%s> with file_size <%d>\n", file->_file_name, file->file_size );
    n_read = fread( &file->file_header, 1, sizeof( sl_file_header_t ), file->_file );
    if ( sizeof( sl_file_header_t ) != n_read )
    {
        LOG_FATAL( "Unable to read header! Attempted to read <%d> bytes, actual read is <%d>\n", sizeof( sl_file_header_t ), n_read );
        return 1;
    }
    LOG_DEBUG( "Format is <%d>, version is <%d>, blocksize is <%d>, file size is <%d>\n", file->file_header.format, file->file_header.version, file->file_header.blocksize, file->file_size );
    return 0;
}


int sl_frame_get_next( sl_file_t *file, sl_frame_t *frame )
{
    size_t n_read;
    n_read = fread( &frame->header, sizeof( sl_frame_header_t ), 1, file->_file );
    if ( sizeof( sl_frame_header_t ) != n_read )
    {
        LOG_FATAL( "Unable to read header! Attempted to read <%d> bytes, actual read is <%d>\n", sizeof( sl_frame_header_t ), n_read );   
        return 1;
    }
    frame->sounder_data = ( uint8_t* ) malloc( frame->header.packet_size );
    if ( NULL == frame->sounder_data )
    {
        LOG_FATAL( "Unable to allocate memory for sounder data!\n" );
        return 1;
    }
    n_read = fread( frame->sounder_data, frame->header.packet_size, 1, file->_file );
    if ( n_read != frame->header.packet_size )
    {
        LOG_FATAL( "Unable to read sounder data!\n" );
        return 1;
    }
    return 0;
}

