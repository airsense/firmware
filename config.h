/*------------------------
*   config.h
*   pin assignments, constants, values, misc. settings
------------------------*/

// generic configs
#define DISP                // defined for unit with lcd display
#define LEDS                // defined for unit with addressable LEDs
#define HEAT                // defined by default, runs heatup routine

#define FONA_RX 2         // obsolete, using hardware serial
#define FONA_TX 3         // obsolete, using hardware serial
#define FONA_RST 4          // fona reset pin
#define FONA_EN             // TODO: hookup enable/disable pin for FONA

#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset in which case, set this #define pin to 0!
#define TFT_DC     8

#define CO    0
#define NO2   1
#define O3    2
#define PM25  3
#define PM10  4

#define PM_25_HRESHOLD 10
#define PM_10_HRESHOLD 10
#define PM_MEAS_TIME 30000
