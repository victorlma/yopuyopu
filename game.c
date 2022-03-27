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

static Rectangle score_rect = {NEXT_BEGIN_POS-2, PADDING * 6, (SQR_SIZE*2)+4, 28*3};
static Color    colors[5] = {LIME, DARKBLUE, GOLD, DARKPURPLE, RED};

static Color blank = BLANK;
bool compare_color(Color *c1, Color *c2){
    if (c1->r == c2->r && c1->g == c2->g && c1->b == c2->b){
        return true;
    }
    else{
        return false;
    }
}

void display_score(game_state_t *game_state){
    int32_t font_size = 22;
    
    char *cur_text =TextFormat("%05i",game_state->score.cur);
    int32_t cur_size = MeasureText(cur_text, font_size);
    DrawText(cur_text,
            NEXT_BEGIN_POS+SQR_SIZE-(cur_size/2), 
            PADDING*6+10, 
            font_size, GREEN);

    cur_text =TextFormat("%05i",game_state->score.prev);
    cur_size = MeasureText(cur_text, font_size);
    DrawText(cur_text,
            NEXT_BEGIN_POS+SQR_SIZE-(cur_size/2), 
            PADDING*7+10, 
            font_size, GRAY);
    cur_text =TextFormat("%05i",game_state->score.total);
    cur_size = MeasureText(cur_text, font_size);
    DrawText(cur_text,
            NEXT_BEGIN_POS+SQR_SIZE-(cur_size/2), 
            PADDING*8+10, 
            font_size, DARKPURPLE);
}

void lose(game_state_t *game_state)
{
    game_state_t gm = {0};
    gm.pl_control = 1;
    gm.board_count = -1;
    gm.score.prev = game_state->score.cur;
    gm.score.total = game_state->score.total;
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
    DrawRectangleRec(score_rect, BLACK);
    DrawRectangleLinesEx(score_rect, LINE_THICK*2, GRAY);
    display_score(game_state);
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

    if (block->stopped && !ignore){
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
    if (block->row == 11) return true;
    if (board_colors[block->row+1][block->col] != 0){
        return true;   
    }
    return false;

}



bool check_end_turn(game_state_t *game_state)
{
    if ((game_state->pl_blk0->del == true || game_state->pl_blk1->del == true) &&
            game_state->matched == 0){
        return true;
    }
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
	    ) || (game_state->pl_blk1->col == 2 && game_state->pl_blk1->row == -2 )
          || (game_state->rot_ind == 0 && game_state->pl_blk0->row > 0 && IsKeyDown(KEY_LEFT) && game_state->board_colors[game_state->pl_blk0->row][game_state->pl_blk0->col-1] != 0)
          || (game_state->rot_ind == 0 && game_state->pl_blk0->row > 0 && IsKeyDown(KEY_RIGHT) && game_state->board_colors[game_state->pl_blk0->row][game_state->pl_blk0->col+1] != 0)
          || (game_state->rot_ind == 2 && game_state->pl_blk1->row > 0 && IsKeyDown(KEY_LEFT) && game_state->board_colors[game_state->pl_blk1->row][game_state->pl_blk1->col-1] != 0)
          || (game_state->rot_ind == 2 && game_state->pl_blk1->row > 0 && IsKeyDown(KEY_RIGHT) && game_state->board_colors[game_state->pl_blk1->row][game_state->pl_blk1->col+1] != 0))
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
        if (IsKeyPressed(KEY_Z)) {
            game_state->pl_control = 0;
            game_state->started = 1;
        }

    }
    else {
    
        if (game_state->nb_count == 4 && !game_state->pl_control){
            
            game_state->pl_next_time += GetFrameTime();
            if (game_state->pl_next_time > 0.2f) {
                game_state->pl_next_time = 0;
                int32_t bc_c;

                for (int i=0; i < ROWS*COLUMNS; ++i){
                    if (compare_color(&game_state->board[i].color, &blank)){
                        game_state->board[i] = game_state->next[0];
                        game_state->board[i].col = 2;
                        game_state->board[i].row = -2;
                        game_state->pl_blk1 = &game_state->board[i];
                        ++game_state->board_count;
                        bc_c = i +1;
                        bc_c = i +1;
                        break;
                    }
                    if (game_state->board[i].del){
                        game_state->board[i] = game_state->next[0];
                        game_state->board[i].col = 2;
                        game_state->board[i].row = -2;
                        game_state->pl_blk1 = &game_state->board[i];
                        bc_c = i +1;
                        break;
                    }
                }


                for (int i=bc_c; i < ROWS*COLUMNS; ++i){
                    if (compare_color(&game_state->board[i].color, &BLANK)){
                        game_state->board[i] = game_state->next[1];
                        game_state->board[i].col = 2;
                        game_state->board[i].row = -1;
                        ++game_state->board_count;
                        game_state->pl_blk0 = &game_state->board[i];
                        break;
                    }
                    if (game_state->board[i].del){
                        game_state->board[i] = game_state->next[1];
                        game_state->board[i].col = 2;
                        game_state->board[i].row = -1;
                        game_state->pl_blk0 = &game_state->board[i];
                        break;
                    }
                }

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
    if (!game_state->board[i].del){
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
    
    if (game_state->pl_blk1->row > -1 && game_state->rot_ind == 2){
        if (btm_coll(game_state->board_colors, game_state->pl_blk1)){
            game_state->pl_blk0->stopped = true;
            game_state->pl_blk1->stopped = true;
        }
    
    }
    if (game_state->pl_blk1->row > -1 && game_state->rot_ind == 0){
        if (btm_coll(game_state->board_colors, game_state->pl_blk0)){
            game_state->pl_blk0->stopped = true;
            game_state->pl_blk1->stopped = true;
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
    if (game_state->pl_down_time > .25f) {
        block_t *pl0;
        block_t *pl1;
        for (int i=0; i < 2; i++) {
            if (i == 0){
                pl0 = game_state->pl_blk0;    
                pl1 = game_state->pl_blk1;    
            }
            else {
            
                pl0 = game_state->pl_blk1;    
                pl1 = game_state->pl_blk0;    
            }
            if (pl0->row <= 0) {
                if (pl0->row == 0) {
                    if (game_state->rot_ind == 0){
                        move_block(game_state, 
                                   pl0,
                                   0, 1, false);
                    }
                    else {
                        move_block(game_state, 
                                   pl1,
                                   0, 1, false);
                    }
                }
                else {
                    pl0->row += 1;
                    if (pl0->row == 0){
                        set_ptr_rxc(game_state, 
                                    pl0); 
                    }
                }
            }
            else {
            
                if (game_state->rot_ind == 0){
                    move_block(game_state, 
                               pl0,
                               0, 1, false);
                }
                else {
                    move_block(game_state, 
                               pl1,
                               0, 1, false);
                }
            }
        
        }
        game_state->pl_down_time = 0;
    }
}


void test_block(game_state_t *game_state, block_t *blk){
    
    if (blk == 0){
        return;
    }
    
    if (game_state->being_matched == 0){
        game_state->being_matched = blk;
        game_state->board_copy[blk->row][blk->col] = 0;
    }
    else if (compare_color(&game_state->being_matched->color, &blk->color)){
        game_state->ml[game_state->matches] = blk;
        game_state->matches += 1;
        game_state->board_copy[blk->row][blk->col] = 0;
        
    }
    else {
        return;
    }

    if (blk->col > 0){
        test_block(game_state, game_state->board_copy[blk->row][blk->col-1]);
    }
    if (blk->col < COLUMNS-1){
        test_block(game_state, game_state->board_copy[blk->row][blk->col+1]); 
    
    }

    if (blk->row > 0){
        test_block(game_state, game_state->board_copy[blk->row-1][blk->col]); 
    }
    if (blk->row < ROWS-1){
        test_block(game_state, game_state->board_copy[blk->row+1][blk->col]);
    }
    if (game_state->matches >= 3 && blk == game_state->being_matched){
        game_state->matched = true;
        for (int i=0; i < game_state->matches; ++i){
            game_state->ml[i]->del = true;
            game_state->board_colors[game_state->ml[i]->row][game_state->ml[i]->col] = 0;

        }
        blk->del = true;
        game_state->board_colors[blk->row][blk->col] = 0;
        game_state->score.cur += (10*game_state->matches)+10;
        game_state->score.total += (10*game_state->matches)+10;
    }
}

void copy_board(game_state_t *game_state){
    for (int j=0; j < ROWS; ++j){
        for (int k=0; k < COLUMNS; ++k){
            game_state->board_copy[j][k] = game_state->board_colors[j][k];
        }
    }
}

void test_match(game_state_t *game_state)
{
    copy_board(game_state);
    
    for (int j=0; j < ROWS; ++j){
        for (int k=0; k < COLUMNS; ++k){
            if (game_state->board_copy[j][k] !=0){
                game_state->being_matched = 0;
                game_state->matches = 0;
                test_block(game_state, game_state->board_copy[j][k]);

            }
        }
    }
}

bool move_all_down(game_state_t *game_state){
    bool retvl = false;

    for (int c=0; c < COLUMNS; ++c){
        for (int r=ROWS-1; r >= 0; --r){
            if (game_state->board_colors[r][c] !=0){
                block_t *blk = game_state->board_colors[r][c];
                if (move_block(game_state, blk, 0,1,1)){
                    retvl = true;
                }
            }
        }
    }
    return retvl;
}

void game_update_drawing(game_state_t *game_state, songs_t *songs) 
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
            
                switch(check_collisions(game_state))
                {
            		case -1:
            		    process_input(game_state);
            		    move_blocks_down(game_state);
            		    break;
            		case 0:
            		    game_state->pl_down_time += GetFrameTime();
            		    if (game_state->pl_down_time > 0.05){
            			move_block(game_state, game_state->pl_blk1, 0, 1, false);
            			game_state->pl_down_time = 0;
            		    }
            		    break;
            		case 1:
            		    game_state->pl_down_time += GetFrameTime();
            		    if (game_state->pl_down_time > 0.05){
            			move_block(game_state, game_state->pl_blk0, 0, 1, false);
            			game_state->pl_down_time = 0;
            		    }
            		    break;
            		case 2:
                        if (game_state->matched){
            		        game_state->pl_down_time += GetFrameTime();
            		        if (game_state->pl_down_time > 0.07){
                                if (!move_all_down(game_state)){
                                    game_state->matched = 0;
                                    test_match(game_state);
                                }
            			        game_state->pl_down_time = 0;
            		        }
                        }else{
                            test_match(game_state);
                        }
            		    break;
                            
                }
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
    	DrawText("\n\nZ and X to Rotate",
    		 SCREEN_WIDTH /1.7  - (MeasureText("Z and X to Rotate", 38)/2)
    		 , SCREEN_HEIGHT - 220, 38, DARKGRAY);
    }
    else {
    	draw_board_entities(game_state);
    }       
    
        
    EndDrawing();
    
    if (game_state->started){
        if (IsMusicStreamPlaying(songs->intro_song)){
            StopMusicStream(*songs->stream);
        }
        songs->stream = &songs->game_song;
        if (!IsMusicStreamPlaying(songs->game_song)){
           PlayMusicStream(*songs->stream);
        }

    }
    else{
    
        if (IsMusicStreamPlaying(songs->game_song)){
            StopMusicStream(*songs->stream);
        }
        songs->stream = &songs->intro_song;
        if (!IsMusicStreamPlaying(songs->intro_song)){
           PlayMusicStream(*songs->stream);
        }
    }
    UpdateMusicStream(*songs->stream);
}
