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

  // --- Role-aware default resolution --------------------------------------
  // Unconfigured means "pick for my role". A solo device has no neighbours to
  // desense, so it keeps full range; meshed roles turn down.
  assert(resolveTxPowerDbm(TX_POWER_AUTO, true)  == WEB_TX_POWER_DBM);
  assert(resolveTxPowerDbm(TX_POWER_AUTO, false) == DEFAULT_TX_POWER_DBM);

  // An explicit choice always wins, including a solo device deliberately
  // turning itself down, or a node deliberately running at full power.
  assert(resolveTxPowerDbm(2, true)   == 2);
  assert(resolveTxPowerDbm(20, false) == 20);
  assert(resolveTxPowerDbm(11, true)  == 11);

  // Values a settings file could not have meant are treated as unconfigured.
  // Older builds auto-create the key with 1, which is below the radio floor.
  assert(resolveTxPowerDbm(1, false)  == DEFAULT_TX_POWER_DBM);
  assert(resolveTxPowerDbm(1, true)   == WEB_TX_POWER_DBM);
  assert(resolveTxPowerDbm(99, true)  == WEB_TX_POWER_DBM);
  assert(resolveTxPowerDbm(-5, false) == DEFAULT_TX_POWER_DBM);

  return 0;
}
