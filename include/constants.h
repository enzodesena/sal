#ifndef CONSTANTS_H
#define CONSTANTS_H


#if defined _WIN32 || defined __CYGWIN__
  #ifdef _EXPORTING
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport)
    #endif
      #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport)
    #endif
  #endif
  #define DLL_HIDDEN
#else
  #if __GNUC__ > 4
    #define DLL_PUBLIC __attribute__ ((visibility("default")))
    #define DLL_HIDDEN __attribute__ ((visibility("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_HIDDEN
  #endif
#endif

enum DLL_PUBLIC InterpolationType {
  kLinear,
  kRounding
};


/** Enum describing whether we are using a right handed or left handed
 reference system. */
enum DLL_PUBLIC Handedness {
  kRightHanded,
  kLeftHanded
};

/** This enum is related to the frequencye absorption characteristics of
 the different walls.  */
enum DLL_PUBLIC WallType {
  kCarpetPile,      /** 6 mm pile carpet bonded to open-cell foam underlay
                     alpha = [0.03,0.09,0.20,0.54,0.70,0.72,0.72];
                     f = [125, 250, 500, 1000, 2000, 4000, 8000]; */
  kCarpetCotton,    /** Cotton carpet
                     alpha = [0.07, 0.31, 0.49, 0.81, 0.66, 0.54, 0.48];
                     f = [125, 250, 500, 1000, 2000, 4000, 8000]; */
  kWallBricks,      /** Walls, hard surfaces average (brick walls, plaster, hard
                     floors, etc.)
                     alpha = [0.02, 0.02, 0.03, 0.03, 0.04, 0.05, 0.05];
                     f = [125, 250, 500, 1000, 2000, 4000, 8000]; */
  kCeilingTile,     /** Fissured ceiling tile
                     alpha = [0.49,0.53,0.53,0.75,0.92,0.99];
                     f = [125, 250, 500, 1000, 2000, 4000]; */
  kRigid            /** Frequency-independent
                     alpha = 0 for all frequencies */
};

#endif /* CONSTANTS_H */
