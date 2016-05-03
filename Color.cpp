#include "Color.h"

float Color::compare(Color *color) {
    float red = this->color[0] - color->color[0];
    float green = this->color[1] - color->color[1];
    float blue = this->color[2] - color->color[2];
    return sqrt(red * red + green + green + blue * blue);
}

