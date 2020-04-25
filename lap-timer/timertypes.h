/*
 * File name:    timertypes.h
 * File Created: 2020-04-25T10:26:25
 */

/*
 * Lap timer types
 */

// Pod messages
enum PodMsg {
  POD_NOTHING = 1,
  POD_GATE,
  POD_HEARTBEAT,
};

// Beam sensor / gate states
enum GateState {
  GATE_OPEN = 1,  // nothing is blocking the beam
  GATE_CLOSED,    // beam is blocked
};

enum State {
  RACE_OVER = 1,  // initial state
  RACE_START,     // race is about to start (beam is blocked)
  RACE_ON,        // race is on!
};
