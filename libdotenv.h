#ifndef libdotenv_dotenv_h
#define libdotenv_dotenv_h

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#   include <windows.h>
#   define r_exists(dotenv_file_path) (GetFileAttributesA((dotenv_file_path)) != INVALID_FILE_ATTRIBUTES ? 0 : -1)
#else
#   include <unistd.h>
#   define r_exists(dotenv_file_path) (access((dotenv_file_path), F_OK) == 0 ? 0 : -1)
#endif

#define r_continue_pointer(pointer) ((*(pointer))++)
#define r_request_pointer(pointer) (*(*(pointer)))

#define r_free_memory(pointer) free((pointer))

#define d_dotenv_line_buffer 256
#define d_default_string_buffer 16

#define r_eof '\0'

static char* f_process_until_eof_char(char** dotenv_content, char r_eof_char) {
    unsigned int iterator = 0;
    unsigned int default_string_buffer = d_default_string_buffer;

    char* request_value = malloc(default_string_buffer * sizeof(char));
    if (request_value == NULL) return NULL;

    while (r_request_pointer(dotenv_content) != r_eof_char && r_request_pointer(dotenv_content) != r_eof) {
        if (iterator >= default_string_buffer - 1) {
            default_string_buffer *= 2;
            char* new_allocated_buffer = realloc(request_value, default_string_buffer * sizeof(char));
            if (new_allocated_buffer == NULL) { r_free_memory(request_value); return NULL; }

            request_value = new_allocated_buffer;
        }

        request_value[iterator++] = r_request_pointer(dotenv_content);
        r_continue_pointer(dotenv_content);
    } request_value[iterator] = r_eof; r_continue_pointer(dotenv_content);

    return request_value;
}

static int f_process_dotenv_inloop(char** dotenv_content, int overwrite_if_exists) {
    char* key_processed = f_process_until_eof_char(dotenv_content, '=');
    if (key_processed == NULL) return -1;

    char* value_processed = f_process_until_eof_char(dotenv_content, '\n');
    if (value_processed == NULL) { r_free_memory(key_processed); return -1; }

    #ifdef _WIN32
        if (_putenv_s(key_processed, value_processed) != 0) {
    #else
        if (setenv(key_processed, value_processed, overwrite_if_exists) != 0) {
    #endif
        r_free_memory(key_processed); r_free_memory(value_processed); return -1;
    }

    r_free_memory(key_processed); r_free_memory(value_processed); return 0;
}

static int f_process_dotenv_content(char** dotenv_content, int overwrite_if_exists) {
    while (r_request_pointer(dotenv_content) != r_eof && r_request_pointer(dotenv_content) != r_eof) {
        if (r_request_pointer(dotenv_content) == '#' || r_request_pointer(dotenv_content) == ';') return 0;
        int inloop_line_status = f_process_dotenv_inloop(dotenv_content, overwrite_if_exists);
        if (inloop_line_status == -1) return -1;
    }

    return 0;
}

int loadenv(const char* dotenv_file_path, int overwrite_if_exists) {
    if (r_exists(dotenv_file_path) != 0) return -1;

    FILE* file_stream = fopen(dotenv_file_path, "r");
    if (file_stream == NULL) return -1;

    char dotenv_line_buffer[d_dotenv_line_buffer] = {0};
    while (fgets(dotenv_line_buffer, sizeof(dotenv_line_buffer), file_stream)) {
        char* dotenv_pointer = dotenv_line_buffer;
        int process_status = f_process_dotenv_content(&dotenv_pointer, overwrite_if_exists);
        if (process_status != 0) return -1;
    } fclose(file_stream);

    return 0;
}

#endif // libdotenv_dotenv_h
