#ifndef _Avion_
#define _Avion_

class Avion {
private:
    float altitude;

    float speed_h;
    float speed_v;
    float angle;

    float dist_cur;
    float dist_min;
    float dist_max;
    float dist_curr;

    int32_t OACI;
    int32_t type;
    char name[9];

    int32_t updates;

    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    bool modified;
    bool GNSS;

public:

    std::vector<float> list_long;
    std::vector<float> list_lat;

    float last_score;
    float mini_score;
    float maxi_score;

public:

    Avion(int32_t _OACI) {
        speed_h  = 0.0f;
        speed_v  = 0.0f;
        angle    = 0.0f;
        dist_cur = 0.0f;
        dist_min = 0.0f;
        dist_max = 0.0f;
        OACI     = _OACI;
        updates  = 0;
        altitude = 0.0f;
        dist_curr = 0.0f;
        modified = false;
        GNSS     = false;
        type     = 0;       // Le type de l'avion ou du moins de l'objet volant !

        mini_score = 1.0f;
        maxi_score = 0.0f;
        last_score = 0.0f;

        for (uint32_t i = 0; i < 8; i += 1)
            name[i] = '-';
        name[8] = 0;
    }

    void set_GNSS_mode(const bool value)
    {
        GNSS = value;
    }

    void update_distance()
    {
        const float lat = get_latitude();
        const float lon = get_longitude();
        dist_curr       = distance(lat, lon, 44.820783, -0.501887);
        if( dist_max != 0 )
        {
            dist_max        = ( dist_max > dist_curr ) ? dist_max : dist_curr;
            dist_min        = ( dist_min < dist_curr ) ? dist_min : dist_curr;
        }else{
            dist_max        = dist_curr;
            dist_min        = dist_curr;
        }
    }

    int32_t get_type() const
    {
        return type;
    }

    void set_type(const int32_t value)
    {
        type = value;
    }

    float get_latitude() const
    {
        if( list_lat.size() != 0 )
            return list_lat.at( list_lat.size() - 1 );
        else
            return 0.0f;
    }

    void set_latitude(const float value)
    {
        list_lat.push_back( value );
    }

    float get_longitude() const
    {
        if( list_long.size() != 0 )
            return list_long.at( list_long.size() - 1 );
        else
            return 0.0f;
    }

    void set_longitude(const float value)
    {
        list_long.push_back( value );
    }

    float get_score() const
    {
        return last_score;
    }

    float get_min_score() const
    {
        return mini_score;
    }

    float get_max_score() const
    {
        return maxi_score;
    }

    void set_score(const float value)
    {
        mini_score = std::min(mini_score, value);
        maxi_score = std::max(maxi_score, value);
        last_score = value;
    }

    float get_speed_horizontal() const
    {
        return speed_h;
    }

    void set_speed_horizontal(const float value)
    {
        speed_h = value;
    }

    float get_speed_vertical() const
    {
        return speed_v;
    }

    void set_speed_vertical(const float value)
    {
        speed_v = value;
    }

    float get_angle() const
    {
        return angle;
    }

    void set_angle(const float value)
    {
        angle = value;
    }

    float get_altitude() const
    {
        return altitude;
    }

    void set_altitude(const float value)
    {
        altitude = value;
    }

    float get_dist_cur() const
    {
        return dist_curr;
    }

    float get_dist_min() const
    {
        return dist_min;
    }

    float get_dist_max() const
    {
        return dist_max;
    }

    int32_t get_OACI() const
    {
        return OACI;
    }

    char *get_name() {
        return name;
    }

    void set_name(const char *value) {
        strcpy(name, value);
    }

    int32_t get_messages() const
    {
        return updates;
    }

    void update()
    {
        lastUpdate = std::chrono::system_clock::now();
        updates += 1;
        modified = true;
    }

    void print()
    {
        if( modified ) green();
        printf("%06X | %s | %s | ", get_OACI(), toCodeName(get_type()), get_name());
        printf("%1.2f [%1.2f, %1.2f] | ", get_score(), get_min_score(), get_max_score());

        if(get_latitude() != 0) printf("%9.6f | %9.6f | ", get_latitude(), get_longitude());
        else                    printf("--------- | --------- | ");

        printf("%4d km/h | %4d m/mn | %4d° | ", (int32_t) get_speed_horizontal(),
               (int32_t) get_speed_vertical(), (int32_t) get_angle());

        if( GNSS ) printf("GNSS | ");
        else       printf("BARO | ");

        if( get_altitude() != 0 ) printf("%5d pds | ",   (int32_t) get_altitude());
        else                      printf("--------- | ");

        printf("%5d km [%3d,%3d] | ", (int32_t) get_dist_cur(), (int32_t) get_dist_min(), (int32_t) get_dist_max());
        printf("%6d | ", get_messages());
        const auto curr   = std::chrono::system_clock::now();
        const int32_t seconds = std::chrono::duration_cast<std::chrono::seconds>(curr - lastUpdate).count();
        if( seconds > 60 )
            printf("%5d mn |\n", seconds/60);
        else
            printf("%6d s |\n", seconds);
        if( modified ) black();
        modified = false;
    }
};

#endif