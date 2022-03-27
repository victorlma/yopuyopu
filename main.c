#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600
#define ROWS        12
#define COLUMNS     6
#define SQR_SIZE    38
#define NEXT_NUM    4
#define LINE_THICK  2
#define PADDING     20

#define NEXT_BEGIN_POS (PADDING + (COLUMNS * SQR_SIZE) + PADDING)


#include "./game.h"
#include "./game.c"


int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "yopuyopu");
    SetTargetFPS(60);   
    InitAudioDevice();
    SetMasterVolume(0.6);
    game_state_t    game_state = {0};   
    game_state.pl_control = 1;
    game_state.board_count = -1;
    songs_t songs;

    songs.intro_song = LoadMusicStream("audio/intro128.mp3");
    songs.intro_song.looping = true;
    songs.game_song = LoadMusicStream("audio/game128.mp3");
    songs.game_song.looping = true;
    while (!WindowShouldClose()) {
                game_update_drawing(&game_state, &songs);       
    }

    UnloadMusicStream(songs.intro_song);
    UnloadMusicStream(songs.game_song);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
