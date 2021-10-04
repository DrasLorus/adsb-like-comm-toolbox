#ifndef _type_aircraft_
#define _type_aircraft_


#define NoInformation            0
#define Reserved                 1
#define SurfaceEmergencyVehicle  2
#define SurfaceServiceVehicle    3
#define GroundObstruction        4
#define Sailplane                5
#define LighterThanAir           5
#define Parachutist              5
#define UltralightAircraft       6
#define UnmannedVehicle          7
#define SpaceVehicle             8
#define LightAircraft            9
#define MediumAircraftCat1      10
#define MediumAircraftCat2      10
#define HighVortexAircraft      11
#define HeavyAircraft           11
#define HighPerformanceAircraft 12
#define Rotorcraft              13


#define str_NoInformation            "NoInfo"
#define str_Reserved                 "Resevd"
#define str_SurfaceEmergencyVehicle  "Emerge"
#define str_SurfaceServiceVehicle    "Surfac"
#define str_GroundObstruction        "Ground"
#define str_Sailplane                " Saild"
#define str_LighterThanAir           " LTAir"
#define str_Parachutist              "Parach"
#define str_UltralightAircraft       " Light"
#define str_UnmannedVehicle          "Unname"
#define str_SpaceVehicle             " Space"
#define str_LightAircraft            " Light"
#define str_MediumAircraftCat1       "Medium"
#define str_MediumAircraftCat2       "Medium"
#define str_HighVortexAircraft       " Heavy"
#define str_HeavyAircraft            " Heavy"
#define str_HighPerformanceAircraft  " Perfo"
#define str_Rotorcraft               " Rotor"


const char* toCodeName(const int32_t value) {
    switch (value) {
        case NoInformation           : return str_NoInformation;
        case Reserved                : return str_Reserved;
        case SurfaceEmergencyVehicle : return str_SurfaceEmergencyVehicle;
        case SurfaceServiceVehicle   : return str_SurfaceServiceVehicle;
        case GroundObstruction       : return str_GroundObstruction;
        case Sailplane               : return str_Sailplane;
//      case LighterThanAir          : return str_LighterThanAir;
//      case Parachutist             : return str_Parachutist;
        case UltralightAircraft      : return str_UltralightAircraft;
        case UnmannedVehicle         : return str_UnmannedVehicle;
        case SpaceVehicle            : return str_SpaceVehicle;
        case LightAircraft           : return str_LightAircraft;
        case MediumAircraftCat1      : return str_MediumAircraftCat1;
//      case MediumAircraftCat2      : return str_MediumAircraftCat2;
        case HighVortexAircraft      : return str_HighVortexAircraft;
//      case HeavyAircraft           : return str_HeavyAircraft;
        case HighPerformanceAircraft : return str_HighPerformanceAircraft;
        case Rotorcraft              : return str_Rotorcraft;
        default                      : return str_NoInformation;
    }
    return str_NoInformation;
}


int32_t toCode(const int32_t tc, const int32_t ca) {
    const int32_t value = 10 * tc + ca;
    switch (value) {
        case  0 : return NoInformation;
        case  1 : return Reserved;
        case  2 : return Reserved;
        case  3 : return Reserved;
        case  5 : return Reserved;
        case  6 : return Reserved;
        case  7 : return Reserved;
        case 10 : return NoInformation;
        case 20 : return NoInformation;
        case 21 : return SurfaceEmergencyVehicle;
        case 23 : return SurfaceServiceVehicle;
        case 24 : return GroundObstruction;
        case 25 : return GroundObstruction;
        case 26 : return GroundObstruction;
        case 27 : return GroundObstruction;
        case 30 : return NoInformation;
        case 31 : return Sailplane;
        case 32 : return LighterThanAir;
        case 33 : return Parachutist;
        case 34 : return UltralightAircraft;
        case 35 : return Reserved;
        case 36 : return UnmannedVehicle;
        case 37 : return SpaceVehicle;
        case 40 : return NoInformation;
        case 41 : return LightAircraft;
        case 42 : return MediumAircraftCat1;
        case 43 : return MediumAircraftCat2;
        case 44 : return HighVortexAircraft;
        case 45 : return HeavyAircraft;
        case 46 : return HighPerformanceAircraft;
        case 47 : return Rotorcraft;
        default : return -1;
    }
    return -1;
}

#endif