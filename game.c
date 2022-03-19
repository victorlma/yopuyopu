#include "./game.h"
static Rectangle  board_rect = {PADDING-2, PADDING-2, (COLUMNS * SQR_SIZE)+4, (ROWS * SQR_SIZE)+4};

static Rectangle  next_rect = {  NEXT_BEGIN_POS-2, PADDING-2,
                                   (SQR_SIZE*2)+4, (SQR_SIZE *2)+4};

static Rectangle    next_positions[NEXT_NUM] = {
    (Rectangle) {NEXT_BEGIN_POS , PADDING , SQR_SIZE, SQR_SIZE},
    (Rectangle) {NEXT_BEGIN_POS , (PADDING + SQR_SIZE) , SQR_SIZE, SQR_SIZE},
    
    (Rectangle) {(NEXT_BEGIN_POS + SQR_SIZE) , PADDING , SQR_SIZE, SQR_SIZE},
    (Rectangle) {(NEXT_BEGIN_POS + SQR_SIZE) , (PADDING + SQR_SIZE) , SQR_SIZE, SQR_SIZE},
};


static Color    colors[5] = {LIME, DARKBLUE, GOLD, PURPLE, RED};





void lose(game_state_t *game_state)
{
    game_state_t gm = {0};
    gm.pl_control = 1;
    gm.board_count = -1;
    *game_state = gm;
}


void fill_next_blocks(game_state_t  *game_state)
{
    if (game_state->nb_count < NEXT_NUM){
        game_state->next[game_state->nb_count].color = colors[GetRandomValue(0,4)];
        ++game_state->nb_count;
    }
}

void draw_next_blocks(game_state_t *game_state)
{
    for (int i = 0; i < NEXT_NUM; ++i) {
        DrawRectangleRec(next_positions[i], game_state->next[i].color);
        DrawRectangleLinesEx(next_positions[i], LINE_THICK, (Color){0,0,0,200});
    }   
}

void draw_board_layer(game_state_t *game_state)
{
    ClearBackground(RAYWHITE);
    DrawRectangleRec(board_rect, GRAY);
    DrawRectangleLinesEx(board_rect, LINE_THICK, BLACK);
    DrawRectangleRec(next_rect, BLACK);
    DrawRectangleLinesEx(next_rect, LINE_THICK, BLACK);

    draw_next_blocks(game_state);
}

void set_ptr_rxc(game_state_t *game_state, block_t *block)
{
    if (block->row >= 0 && block->row < ROWS && block->col >=0 && block->col < COLUMNS){
        game_state->board_colors[block->row][block->col] = block;
    }
}


bool move_block(game_state_t *game_state, block_t *block, int32_t nc, int32_t nr, bool ignore)
{
    int32_t ncol = block->col + nc;
    int32_t nrow = block->row + nr;

    if (block->stopped){
        return false;
    }

    if (block->row < 0){
        if (game_state->board_colors[0][ncol] == 0){
        block->col = ncol < COLUMNS && ncol >=0 ? ncol : block->col; 
        }
    }
    if (nrow >= 0 && ncol >= 0 && nrow < ROWS && ncol < COLUMNS &&
        game_state->board_colors[nrow][ncol] == 0) {
        game_state->board_colors[block->row][block->col] = 0;
        
        block->col = ncol;
        block->row = nrow;
        
        game_state->board_colors[block->row][block->col] = block;
        return true;
    }
    return false;
}


bool btm_coll(block_t *board_colors[ROWS][COLUMNS], block_t *block)
{
    if (board_colors[block->row+1][block->col] != 0){
        return true;   
    }
    return false;

}



bool check_end_turn(game_state_t *game_state)
{
    if (game_state->pl_blk0->row == ROWS -1 || game_state->pl_blk1->row == ROWS -1 ) {
        game_state->pl_turn_time += GetFrameTime();
        if (game_state->pl_turn_time > 0.3f) {
            return true;
        }
        else { return false;}
    }
    else{





        if (btm_coll(game_state->board_colors, game_state->pl_blk0) &&
            btm_coll(game_state->board_colors, game_state->pl_blk1))
        {
            
            
            if (game_state->pl_blk1->col == 2 && game_state->pl_blk1->row == -1) {
                lose(game_state);
                return true;
            }
            
        

            game_state->pl_turn_time += GetFrameTime(); 
            if (game_state->pl_turn_time > 0.3f) {
                return true;
            
            }
            else { return false;}
        }
    }
    return false;
}


void rotate_block(game_state_t *game_state, int32_t dir)
{
    bool moved;
    if (game_state->pl_blk0->row >= 0){
        block_t * blk_m = game_state->pl_blk1;
        switch (game_state->rot_ind) {
            
            case 0:
                moved = move_block(game_state, blk_m, 0 + dir, 1, false);
                break;
            case 1:
                
                moved = move_block(game_state, blk_m, -1, 0 + dir, false);
                break;
            case 2:
                
                moved = move_block(game_state, blk_m, 0 - dir, -1, false);
                break;
            case 3:
                moved = move_block(game_state, blk_m, 1, 0 - dir, false);
                break;
        
        }
        if (moved)
        {
          if (dir == 1) {
            if (++game_state->rot_ind > 3) game_state->rot_ind = 0;
            
          }
          else {
            if (--game_state->rot_ind < 0) game_state->rot_ind = 3;
          }
        }
    }
}

void process_input(game_state_t *game_state)
{

    if (IsKeyPressed(KEY_Z) && game_state->pl_blk1->row > -2) 
    {
            rotate_block(game_state, -1);
        game_state->pl_down_time = 0;
    }
    if (IsKeyPressed(KEY_X) && game_state->pl_blk1->row > -2) 
    {
            rotate_block(game_state, 1);
        game_state->pl_down_time = 0;
    }


    if (!IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_DOWN)) {
        game_state->pl_down_time += 0.2f;
    }
    
    if (!IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT))
    {
        game_state->pl_side_time = 0;
    }


    if ((game_state->pl_blk1->col == 2 && game_state->pl_blk1->row == -1 && (
        game_state->board_colors[game_state->pl_blk0->row+1][game_state->pl_blk0->col] != 0 &&
        game_state->board_colors[game_state->pl_blk1->row+1][game_state->pl_blk1->col] != 0 )
        ) || (game_state->pl_blk1->col == 2 && game_state->pl_blk1->row == -2 ))
    {;}
    else {          
    if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)) {
        
        if (game_state->pl_side_time <= 0) {        
            if (game_state->rot_ind == 3) {
        
                move_block(game_state,
                           game_state->pl_blk0, 1, 0, false);
                move_block(game_state,
                           game_state->pl_blk1, 1, 0, false);
            }
            else{   
                move_block(game_state,
                           game_state->pl_blk1, 1, 0, false);
                move_block(game_state,
                           game_state->pl_blk0, 1, 0, false);
            }
            game_state->pl_side_time = .8f;
            
        }
        else { game_state->pl_side_time -= 0.15f;}
    }
    
    if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
        if (game_state->pl_side_time <= 0) {        
            
            if (game_state->rot_ind == 1){
                move_block(game_state,
                           game_state->pl_blk0, -1, 0, false);
                move_block(game_state,
                           game_state->pl_blk1, -1, 0, false);
            }
            else {
                move_block(game_state,
                           game_state->pl_blk1, -1, 0, false);
                move_block(game_state,
                           game_state->pl_blk0, -1, 0, false);
            
            }
            
            
            game_state->pl_side_time = .8f;
        }   
        else { game_state->pl_side_time -= 0.15f;}
    }
    }
}

void init_turn(game_state_t *game_state)
{

    if (!game_state->started){
        game_state->pl_next_time += GetFrameTime();
        if (game_state->pl_next_time > 3.0f || IsKeyPressed(KEY_Z)) {
            game_state->pl_next_time = 0;
            game_state->pl_control = 0;
            game_state->started = 1;
        }

    }
    else {
    
        if (game_state->nb_count == 4 && !game_state->pl_control){
            
            game_state->pl_next_time += GetFrameTime();
            if (game_state->pl_next_time > 0.2f) {
                game_state->pl_next_time = 0;

                ++game_state->board_count;
                game_state->board[game_state->board_count] = game_state->next[0];
                game_state->board[game_state->board_count].col = 2;
                game_state->board[game_state->board_count].row = -2;
                ++game_state->board_count;
                game_state->board[game_state->board_count] = game_state->next[1];
                game_state->board[game_state->board_count].col = 2;
                game_state->board[game_state->board_count].row = -1;
                game_state->pl_blk0 = &game_state->board[game_state->board_count];
                game_state->pl_blk1 = &game_state->board[game_state->board_count-1];
                game_state->next[0] = game_state->next[2];
                game_state->next[1] = game_state->next[3];
                game_state->nb_count = 2;
                game_state->pl_control = 1;
                game_state->rot_ind = 0;
                game_state->pl_turn_time = 0;   
            }

        }
    
    }

    fill_next_blocks(game_state);
}

void draw_board_entities(game_state_t *game_state)
{

        for (int i=0; i <= game_state->board_count; ++i) {
            Rectangle   block_rect;
            if (game_state->board[i].row < 0) {
                block_rect =  (Rectangle) { 
                    ( PADDING + (game_state->board[i].col * SQR_SIZE)),
                    ( PADDING*2 + (game_state->board[i].row * SQR_SIZE)),
                    SQR_SIZE, SQR_SIZE
                };
            
            }
            else {
                block_rect = (Rectangle) {
                    (PADDING + (game_state->board[i].col * SQR_SIZE)),
                    (PADDING + (game_state->board[i].row * SQR_SIZE)),
                    SQR_SIZE, SQR_SIZE
                };
            }
            
            DrawRectangleRec(block_rect, game_state->board[i].color);
            DrawRectangleLinesEx(block_rect, LINE_THICK, BLACK);
        }
}


int32_t check_collisions(game_state_t *game_state){
    if (game_state->pl_blk1->row > -1 && 
            (game_state->rot_ind == 1 || game_state->rot_ind == 3)){

       
        if (btm_coll(game_state->board_colors,game_state->pl_blk1)){
            game_state->pl_blk1->stopped = true;
        }
        if (btm_coll(game_state->board_colors,game_state->pl_blk0)){
            game_state->pl_blk0->stopped = true;
        }
    }
    

    if (game_state->pl_blk0->stopped && !game_state->pl_blk1->stopped){
        return 0;
    }
    else if (game_state->pl_blk1->stopped && !game_state->pl_blk0->stopped){
        return 1;
    }
    else if (game_state->pl_blk0->stopped && game_state->pl_blk1->stopped){
        return 2;
    }
    else {
        return -1;
    }
}


void move_blocks_down(game_state_t *game_state)
{

    game_state->pl_down_time += GetFrameTime();
    if (game_state->pl_down_time > .3f) {
    
        for (int i=0; i < 2; i++) {
            if (game_state->board[game_state->board_count-i].row <= 0) {
                if (game_state->board[game_state->board_count-i].row == 0) {
                    if (game_state->rot_ind == 0){
                        move_block(game_state, 
                                   &game_state->board[game_state->board_count-i],
                                   0, 1, false);
                    }
                    else {
                        move_block(game_state, 
                                   game_state->pl_blk1+i,
                                   0, 1, false);
                    }
                }
                else {
                    game_state->board[game_state->board_count-i].row += 1;
                    if (game_state->board[game_state->board_count-i].row == 0){
                        set_ptr_rxc(game_state, 
                                    &game_state->board[game_state->board_count-i]); 
                    }
                }
            }
            else {
            
                if (game_state->rot_ind == 0){
                    move_block(game_state, 
                               &game_state->board[game_state->board_count-i],
                               0, 1, false);
                }
                else {
                    move_block(game_state, 
                               game_state->pl_blk1+i,
                               0, 1, false);
                }
            }
        
        }
        game_state->pl_down_time = 0;
    }
}

void game_update_drawing(game_state_t *game_state) 
{
    
    
    init_turn(game_state);  

    if (!game_state->started) {
    }
    else {

        if (game_state->pl_control) {
            
            if (check_end_turn(game_state)) {
                game_state->pl_control = 0;
            }       
            else {
            
                process_input(game_state);
                move_blocks_down(game_state);
                check_collisions(game_state);
            }
        }
    }
    
    BeginDrawing();

        draw_board_layer(game_state);
        if (!game_state->started) {
            DrawText("Game Starting !!",
                    SCREEN_WIDTH /1.7  - (MeasureText("Game Starting!!", 38)/2)
                    , SCREEN_HEIGHT - 220, 38, DARKGRAY);
            DrawText("\nArrow keys to MOVE",
                    SCREEN_WIDTH /1.7  - (MeasureText("Arrow keys to MOVE", 38)/2)
                    , SCREEN_HEIGHT - 220, 38, GREEN);
            DrawText("\n\nZ to Rotate",
                    SCREEN_WIDTH /1.7  - (MeasureText("Z to Rotate", 38)/2)
                    , SCREEN_HEIGHT - 220, 38, DARKGRAY);
        }
        else {
            draw_board_entities(game_state);
        }       
    
        
    EndDrawing();
    
}
