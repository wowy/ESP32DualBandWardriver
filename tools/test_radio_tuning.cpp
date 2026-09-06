#include <assert.h>
#include <stdint.h>

#include "RadioTuning.h"

int main() {
  // --- Rendezvous stagger -------------------------------------------------
  // Each node waits a distinct slice of the rendezvous window before it
  // transmits, so overlapping windows do not put heartbeats on top of
  // each other.
  assert(calculateNodeStaggerOffsetMs(1, 4, 400) == 100);
  assert(calculateNodeStaggerOffsetMs(2, 4, 400) == 200);
  assert(calculateNodeStaggerOffsetMs(3, 4, 400) == 300);
  assert(calculateNodeStaggerOffsetMs(0, 4, 400) == 0);

  // Uneven divisions truncate rather than overrun the window.
  assert(calculateNodeStaggerOffsetMs(1, 3, 300) == 100);
  assert(calculateNodeStaggerOffsetMs(2, 3, 300) == 200);
  assert(calculateNodeStaggerOffsetMs(2, 7, 300) == 85);

  // A sole node has nobody to contend with, so it never delays.
  assert(calculateNodeStaggerOffsetMs(0, 1, 400) == 0);

  // Defensive cases: an unassigned or inconsistent node must not stall.
  assert(calculateNodeStaggerOffsetMs(0, 0, 400) == 0);
  assert(calculateNodeStaggerOffsetMs(5, 4, 400) == 0);
  assert(calculateNodeStaggerOffsetMs(4, 4, 400) == 0);
  assert(calculateNodeStaggerOffsetMs(1, 4, 0) == 0);

  // The offset always leaves room inside the window for the transmission.
  assert(calculateNodeStaggerOffsetMs(23, 24, 300) < 300);

  // --- TX power quantisation ----------------------------------------------
  // esp_wifi_set_max_tx_power takes 0.25 dBm units over [8, 84], and only
  // maps to a fixed ladder of actual outputs. Never exceed what was asked.
  assert(txPowerDbmToQuarterDbm(2) == 8);
  assert(txPowerDbmToQuarterDbm(5) == 20);
  assert(txPowerDbmToQuarterDbm(7) == 28);
  assert(txPowerDbmToQuarterDbm(8) == 34);
  assert(txPowerDbmToQuarterDbm(11) == 44);
  assert(txPowerDbmToQuarterDbm(13) == 52);
  assert(txPowerDbmToQuarterDbm(14) == 56);
  assert(txPowerDbmToQuarterDbm(15) == 60);
  assert(txPowerDbmToQuarterDbm(16) == 66);
  assert(txPowerDbmToQuarterDbm(18) == 72);
  assert(txPowerDbmToQuarterDbm(20) == 80);

  // Values between rungs round down, so a cap is never overshot.
  assert(txPowerDbmToQuarterDbm(4) == 8);
  assert(txPowerDbmToQuarterDbm(6) == 20);
  assert(txPowerDbmToQuarterDbm(19) == 72);

  // Out-of-range requests clamp to the hardware limits.
  assert(txPowerDbmToQuarterDbm(1) == 8);
  assert(txPowerDbmToQuarterDbm(0) == 8);
  assert(txPowerDbmToQuarterDbm(-40) == 8);
  assert(txPowerDbmToQuarterDbm(21) == 80);
  assert(txPowerDbmToQuarterDbm(127) == 80);

  return 0;
}
