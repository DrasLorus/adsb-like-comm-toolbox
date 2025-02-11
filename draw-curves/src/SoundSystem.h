#ifndef __C_SoundSystem__
#define __C_SoundSystem__

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "SoundView.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

class SoundSystem : public sf::Drawable, public sf::Transformable
{
public:
    SoundSystem    (std::vector<int16_t>& buffer);
    void update_ref(std::vector<int16_t>& buffer);
    void update_mod(std::vector<int16_t>& buffer);
    void zoom_in();
    void zoom_out();
    void slide_left();
    void slide_right();
    void update_fName(std::string _name);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    std::string _fName;
    vector<SoundView*> liste;
};

#endif
