#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef GAME_H_
#define GAME_H_
typedef struct {
    int32_t col;
    int32_t row;
    bool    stopped;
    bool    del;
    Color   color;

} block_t;

typedef struct {
    int32_t     cur;
    int32_t     prev;
    int32_t     total;
} score_t;
typedef struct {
    Music   intro_song;
    Music   game_song;
    Music   *stream;
} songs_t;
typedef struct {
    int32_t     nb_count;
    int32_t     board_count;
    int32_t     rot_ind;
    int32_t     matches;
    bool        pl_control;
    bool        started;
    bool        matched;
    float       pl_next_time;
    float       pl_down_time;
    float       pl_side_time;
    float       pl_turn_time;
    block_t     next[NEXT_NUM];
    block_t     board[ROWS*COLUMNS*2];
    block_t    *board_colors[ROWS][COLUMNS];
    block_t    *board_copy[ROWS][COLUMNS];
    block_t     *ml[ROWS*COLUMNS]; //matched_list
    block_t     *pl_blk0;
    block_t     *pl_blk1;
    block_t     *being_matched;
    score_t       score;
} game_state_t;
#endif
