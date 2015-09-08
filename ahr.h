#ifndef AHR
#define AHR

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

#define RETURN_ERROR -1

#define DEFENCE_DISTANCE 50

enum BoardPosition  {
    LEFT_TOP,
    RIGHT_TOP,
    RIGHT_BOTTOM,
    LEFT_BOTTOM,
    END,
    RESET_BOARD_AREA
};

enum BallDirection  {
    PAUSE,
    NORTH_EAST,
    SOUTH_EAST,
    SOUTH_WEST,
    NORTH_WEST
};

enum RobotSide {
    LEFT_SIDE,
    RIGHT_SIDE
};

#endif // AHR

