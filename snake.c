#include <snake.h>

int snake[15][25] = {
    {0, 0, 0, 0, 0,
    0, 2, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 3, 0, 0,
    0, 0, 3, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 2, 0, 0, 0,
    0, 0, 3, 0, 0,
    0, 0, 3, 0, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 2, 3, 0, 0,
    0, 0, 3, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 3, 3, 0, 0,
    0, 0, 3, 0, 0,
    0, 0, 0, 2, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 3, 3, 0, 0,
    0, 3, 0, 0, 0,
    0, 0, 0, 2, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 3, 0, 0, 0,
    0, 3, 0, 0, 0,
    0, 3, 0, 2, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 3, 0, 0, 0,
    0, 3, 3, 2, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 0, 2, 0, 0,
    0, 3, 0, 0, 0,
    0, 3, 3, 3, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 0, 2, 0, 0,
    0, 0, 0, 3, 0,
    0, 3, 3, 3, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 0, 2, 0, 0,
    0, 0, 3, 3, 0,
    0, 0, 3, 3, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 0, 3, 0, 2,
    0, 0, 3, 3, 0,
    0, 0, 3, 3, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 0, 3, 3, 2,
    0, 0, 3, 3, 0,
    0, 0, 0, 3, 0,
    0, 0, 0, 0, 0,
    },

    {0, 0, 0, 0, 0,
    0, 0, 3, 3, 3,
    0, 0, 3, 3, 0,
    0, 0, 0, 3, 0,
    0, 0, 0, 0, 0,
    },

    {2, 2, 2, 2, 2,
    2, 2, 2, 2, 2,
    2, 2, 2, 2, 2,
    2, 2, 2, 2, 2,
    2, 2, 2, 2, 2,
    },

    {0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    }
};
#include "snake.h"

int snake2[15][25] = {{0, 0, 0, 0, 0,
                    2, 0, 0, 0, 0,
                    0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0},
    
                    {0, 0, 0, 0, 0,
                    2, 0, 0, 0, 0,
                    0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 1, 0},

                        {0, 0, 0, 0, 0,
                        2, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 1, 0},

                        {0, 0, 0, 0, 0,
                        2, 0, 0, 0, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 1, 0},

                        {0, 0, 0, 0, 0,
                        2, 0, 0, 1, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 1, 0},

                        {0, 0, 0, 0, 0,
                        2, 0, 1, 1, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0},

                        {0, 0, 0, 0, 0,
                        2, 1, 1, 1, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0},

                        {0, 0, 0, 0, 2,
                        1, 1, 1, 1, 0,
                        0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0},

                        {1, 0, 0, 0, 2,
                        1, 1, 1, 1, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0},

                        {1, 0, 0, 0, 2,
                        1, 1, 1, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0},

                        {1, 0, 0, 0, 2,
                        1, 1, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0},

                        {1, 0, 0, 0, 2,
                        1, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0},

                        {1, 0, 0, 0, 2,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0},

                        {0, 0, 0, 0, 2,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0},

                        {3, 3, 3, 3, 3,
                        3, 3, 3, 3, 3,
                        3, 3, 3, 3, 3,
                        3, 3, 3, 3, 3,
                        3, 3, 3, 3, 3}
                    
                    };
