#ifndef PARAMS_H
#define PARAMS_H

namespace DigitalEvolution
{

const int ENERGY_NEED = 5;
const float TRANSPORT_ENERGY_PROPORTION = 1;// доля энергии к передаче
const int ENERGY_START = 2000;
const int ENERGY_NEED_TO_BORN = 10;
const int ENERGY_GENERATE = 120;
const int TEMP_MAX_ENERGY = 2000;

enum class Direction : int {None = -1, Left = 0, Up = 1, Right = 2, Down = 3};
enum class TransportPolicy {None, Сonsumer, Source, Transporter};

inline Direction mirrorDirection(Direction direction)
{
    int x = static_cast<int>(direction);
    if (x >= 0)
        return static_cast<Direction>((x + 2) % 4);
    return direction;
}


}


#endif // PARAMS_H
