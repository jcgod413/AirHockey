#ifndef AHR
#define AHR

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

#define RETURN_ERROR -1

#define DEFENCE_DISTANCE 50
#define SERIAL_DELAYTIME 50
#define SCREEN_DATA_LENGTH SCREEN_WIDTH*SCREEN_HEIGHT*4

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

enum RadioState {
    RADIO_BALL,
    RADIO_ROBOT
};

enum Packet_Major   {
    MOVE
};

enum Packet_Minor {
    ERROR,
    DIRECTION_SOUTH_WEST,
    DIRECTION_SOUTH,
    DIRECTION_SOUTH_EAST,
    DIRECTION_EAST,
    DIRECTION_HALT,
    DIRECTION_WEST,
    DIRECTION_NORTH_WEST,
    DIRECTION_NORTH,
    DIRECTION_NORTH_EAST
};

#endif // AHR

