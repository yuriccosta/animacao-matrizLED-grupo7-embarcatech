#include "tetris.h"

int desenho[15][25] =   {{0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 4,
                        0, 1, 0, 0, 4,
                        1, 1, 1, 0, 4},

			            {2, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 4,
                        0, 1, 0, 0, 4,
                        1, 1, 1, 0, 4},

			            {2, 0, 0, 0, 0,
                        2, 0, 0, 0, 0, 
                        0, 0, 0, 0, 4,
                        0, 1, 0, 0, 4,
                        1, 1, 1, 0, 4},

			            {2, 2, 0, 0, 0,
                        2, 0, 0, 0, 0, 
                        2, 0, 0, 0, 4,
                        0, 1, 0, 0, 4,
                        1, 1, 1, 0, 4},

			            {0, 0, 0, 0, 0,
                        2, 2, 2, 0, 0, 
                        0, 0, 2, 0, 4,
                        0, 1, 0, 0, 4,
                        1, 1, 1, 0, 4},

			            {0, 0, 0, 0, 0,
                        0, 2, 2, 2, 0, 
                        0, 0, 0, 2, 4,
                        0, 1, 0, 0, 4,
                        1, 1, 1, 0, 4},

			            {0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 
                        0, 2, 2, 2, 4,
                        0, 1, 0, 2, 4,
                        1, 1, 1, 0, 4},

			            {3, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 
                        0, 2, 2, 2, 4,
                        0, 1, 0, 2, 4,
                        1, 1, 1, 0, 4},

			            {3, 3, 0, 0, 0,
                        3, 0, 0, 0, 0, 
                        0, 2, 2, 2, 4,
                        0, 1, 0, 2, 4,
                        1, 1, 1, 0, 4},

			            {0, 0, 0, 0, 0,
                        3, 3, 0, 0, 0, 
                        3, 2, 2, 2, 4,
                        0, 1, 0, 2, 4,
                        1, 1, 1, 0, 4},

			            {0, 0, 0, 0, 0,
                        3, 3, 0, 0, 0, 
                        0, 0, 0, 0, 0,
                        0, 1, 0, 2, 4,
                        1, 1, 1, 0, 4},

			            {0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 
                        3, 3, 0, 0, 0,
                        0, 1, 0, 0, 4,
                        1, 1, 1, 2, 4},

			            {0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 
                        0, 3, 0, 0, 0,
                        3, 1, 0, 0, 4,
                        0, 0, 0, 0, 0},

			            {0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0,
                        0, 3, 0, 0, 0,
                        3, 1, 0, 0, 4},

   			            {0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0}};