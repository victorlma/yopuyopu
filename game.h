typedef struct {
	int32_t	col;
	int32_t	row;
	Color	color;
} block_t;


typedef struct {
	int32_t		nb_count;
	int32_t		board_count;
	int32_t		rot_ind;
	bool		pl_control;
	bool		started;
	float		pl_next_time;
	float		pl_down_time;
	float		pl_side_time;
	float		pl_turn_time;
	block_t		next[NEXT_NUM];
	block_t		board[ROWS*COLUMNS];
	block_t    *board_colors[ROWS][COLUMNS];
	block_t		*pl_blk0;
	block_t		*pl_blk1;
} game_state_t;

