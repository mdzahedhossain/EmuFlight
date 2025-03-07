/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdbool.h>
#include "common/time.h"
#include "common/filter.h"
#include "pg/pg.h"

#define MAX_PID_PROCESS_DENOM       16
#define PID_CONTROLLER_BETAFLIGHT   1
#define PID_MIXER_SCALING           1000.0f
#define PID_SERVO_MIXER_SCALING     0.7f
#define PIDSUM_LIMIT                500
#define PIDSUM_LIMIT_YAW            400
#define PIDSUM_LIMIT_MIN            100
#define PIDSUM_LIMIT_MAX            1000
#define KD_RING_BUFFER_SIZE 10
#define EMU_GRAVITY_THROTTLE_FILTER_CUTOFF 15  // The emu gravity throttle highpass filter cutoff


// Scaling factors for Pids for better tunable range in configurator for betaflight pid controller. The scaling is based on legacy pid controller or previous float
#define PTERM_SCALE 0.032029f
#define ITERM_SCALE 0.244381f
#define DTERM_SCALE 0.000529f
#define DIRECT_FF_SCALE 0.005f

typedef enum {
    PID_ROLL,
    PID_PITCH,
    PID_YAW,
    PID_LEVEL_LOW, //pid controller for low errorAngle
    PID_LEVEL_HIGH, //pid controller for high errorAngle
    PID_MAG,
    PID_ITEM_COUNT
} pidIndex_e;

typedef enum {
    SUPEREXPO_YAW_OFF = 0,
    SUPEREXPO_YAW_ON,
    SUPEREXPO_YAW_ALWAYS
} pidSuperExpoYaw_e;

typedef enum {
    PID_STABILISATION_OFF = 0,
    PID_STABILISATION_ON
} pidStabilisationState_e;

typedef enum {
    PID_CRASH_RECOVERY_OFF = 0,
    PID_CRASH_RECOVERY_ON,
    PID_CRASH_RECOVERY_BEEP
} pidCrashRecovery_e;

typedef enum {
    MIXER_IMPL_LEGACY = 0,
    MIXER_IMPL_SMOOTH,
    MIXER_IMPL_2PASS,
    MIXER_IMPL_COUNT
} mixerImplType_e;

typedef struct pidf_s {
    uint8_t P;
    uint8_t I;
    uint8_t D;
    uint16_t F; // Only angle mode still uses FF

} pidf_t;

typedef struct dFilter_s {
    uint16_t dLpf;
    uint16_t dLpf2;
} dFilter_t;

typedef struct pidProfile_s {
    pidf_t  pid[PID_ITEM_COUNT];
    uint8_t directFF_yaw;                    // a true FF for the yaw
    dFilter_t dFilter[3];

    uint8_t dterm_filter_type;              // Filter selection for dterm
    uint8_t dterm_filter2_type;              // Filter selection for dterm
    uint8_t itermWindupPointPercent;        // Experimental ITerm windup threshold, percent motor saturation
    uint16_t pidSumLimit;
    uint16_t pidSumLimitYaw;
    uint8_t pidAtMinThrottle;               // Disable/Enable pids on zero throttle. Normally even without airmode P and D would be active.
    uint8_t levelAngleLimit;                // Max angle in degrees in level mode
    uint8_t angleExpo;                      // How much expo to add to the
    uint8_t angle_filter;                   // filter we stick on the rate setpoint angle makes

    uint8_t horizonTransition;              // horizonTransition
    uint8_t horizon_tilt_effect;            // inclination factor for Horizon mode
    uint8_t horizonStrength;                // boost or shrink to angle pids while in horizon mode

    // EmuFlight PID controller parameters
    uint8_t feathered_pids;                 // determine how feathered your pids are
    uint8_t i_decay;                        // i-term decay (increases how quickly iterm shrinks in value)
    uint8_t i_decay_cutoff;                 // iterm values above which i_decay has full effect
    uint16_t errorBoost;                    // the weight of the error boost
    uint16_t errorBoostYaw;                 // the weight of the error boost for yaw
    uint8_t errorBoostLimit;                // percentage of the error that the emu boost can boost
    uint8_t errorBoostLimitYaw;             // percentage of the error that the emu boost can boost for yaw
    uint16_t dtermBoost;                    // Dterm boost similar to EmuBoost
    uint8_t dtermBoostLimit;                // Max value allowed for the dterm boost
    uint16_t yawRateAccelLimit;             // yaw accel limiter for deg/sec/ms
    uint16_t rateAccelLimit;                // accel limiter roll/pitch deg/sec/ms
    uint16_t crash_dthreshold;              // dterm crash value
    uint16_t crash_gthreshold;              // gyro crash value
    uint16_t crash_setpoint_threshold;      // setpoint must be below this value to detect crash, so flips and rolls are not interpreted as crashes
    uint16_t crash_time;                    // ms
    uint16_t crash_delay;                   // ms
    uint8_t crash_recovery_angle;           // degrees
    uint8_t crash_recovery_rate;            // degree/second
    uint8_t setPointPTransition[3];         // SPA p transition
    uint8_t setPointITransition[3];         // SPA i transition
    uint8_t setPointDTransition[3];         // SPA d transition
    uint16_t crash_limit_yaw;               // limits yaw errorRate, so crashes don't cause huge throttle increase
    uint16_t itermLimit;                    // Maximum value that the iterm can accumulate to
    uint8_t crash_recovery;                 // off, on, on and beeps when it is in crash recovery mode
    uint8_t throttle_boost;                 // how much should throttle be boosted during transient changes 0-100, 100 adds 10x hpf filtered throttle
    uint8_t throttle_boost_cutoff;          // Which cutoff frequency to use for throttle boost. higher cutoffs keep the boost on for shorter. Specified in hz.
    uint8_t iterm_rotation;                 // rotates iterm to translate world errors to local coordinate system
    uint8_t iterm_relax_cutoff;
    uint8_t iterm_relax_cutoff_yaw;
    uint8_t iterm_relax_threshold;          // This cutoff frequency specifies a low pass filter which predicts average response of the quad to setpoint
    uint8_t iterm_relax_threshold_yaw;      // This cutoff frequency specifies a low pass filter which predicts average response of the quad to setpoint
    uint8_t motor_output_limit;             // Upper limit of the motor output (percent)
    int8_t auto_profile_cell_count;         // Cell count for this profile to be used with if auto PID profile switching is used
    uint8_t emuGravityGain;                // Gain for the EmuGravity
    uint8_t axis_lock_hz;                   // filter for the axis lock
    uint8_t axis_lock_multiplier;           // multplier for the axis lock effect
    uint8_t linear_thrust_low_output;       // Sets the level of thrust linearization for low motor outputs
    uint8_t linear_thrust_high_output;      // Sets the level of thrust linearization for high motor outputs
    uint8_t linear_throttle;                // When thrust linearization is enabled, tells whether the throttle has to be linear or counter-compensated for legacy feedback
    mixerImplType_e mixer_impl;             // Which mixer implementation use
    uint8_t mixer_laziness;                 // If enabled, mixer clipping strategy will shift values only by the minimum required amount per motor group. Requires linear thrust
    uint8_t mixer_yaw_throttle_comp;        // If enabled, throttle compesesation for quick yaw movement in 2pass mixer
    uint16_t dterm_ABG_alpha;
    uint16_t dterm_ABG_boost;
    uint8_t dterm_ABG_half_life;
    uint8_t dtermDynNotch;
    uint16_t dterm_dyn_notch_q;
} pidProfile_t;

#ifndef USE_OSD_SLAVE
PG_DECLARE_ARRAY(pidProfile_t, PID_PROFILE_COUNT, pidProfiles);
#endif

typedef struct pidConfig_s {
    uint8_t pid_process_denom;                   // Processing denominator for PID controller vs gyro sampling rate
    uint8_t runaway_takeoff_prevention;          // off, on - enables pidsum runaway disarm logic
    uint16_t runaway_takeoff_deactivate_delay;   // delay in ms for "in-flight" conditions before deactivation (successful flight)
    uint8_t runaway_takeoff_deactivate_throttle; // minimum throttle percent required during deactivation phase
} pidConfig_t;

PG_DECLARE(pidConfig_t, pidConfig);

union rollAndPitchTrims_u;
void pidController(const pidProfile_t *pidProfile, const union rollAndPitchTrims_u *angleTrim, timeUs_t currentTimeUs);

typedef struct pidAxisData_s {
    float P;
    float I;
    float D;

    float Sum;
} pidAxisData_t;

extern const char pidNames[];

extern pidAxisData_t pidData[3];

extern uint32_t targetPidLooptime;

extern float throttleBoost;
extern pt1Filter_t throttleLpf;

void pidResetITerm(void);
void pidStabilisationState(pidStabilisationState_e pidControllerState);
void pidInitFilters(const pidProfile_t *pidProfile);
void pidInitConfig(const pidProfile_t *pidProfile);
void pidInit(const pidProfile_t *pidProfile);
void pidCopyProfile(uint8_t dstPidProfileIndex, uint8_t srcPidProfileIndex);
bool crashRecoveryModeActive(void);
float pidGetPreviousSetpoint(int axis);
void pidUpdateEmuGravityThrottleFilter(float throttle);
