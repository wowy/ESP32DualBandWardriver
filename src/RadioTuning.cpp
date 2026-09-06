#include "RadioTuning.h"

uint32_t calculateNodeStaggerOffsetMs(uint8_t node_index,
                                      uint8_t node_count,
                                      uint32_t window_ms) {
  // A lone node owns the channel, and an index outside the assignment means
  // core has not placed this node yet. Neither case should delay anything.
  if ((node_count <= 1) || (node_index >= node_count))
    return 0;

  // Truncating division keeps the last slot strictly inside the window.
  return ((uint32_t)node_index * window_ms) / node_count;
}

int8_t resolveTxPowerDbm(int8_t stored, bool is_solo) {
  // A value the radio cannot produce cannot be a deliberate choice, so treat
  // it as unconfigured rather than clamping it into something the user never
  // asked for.
  if ((stored < MIN_TX_POWER_DBM) || (stored > MAX_TX_POWER_DBM))
    return is_solo ? WEB_TX_POWER_DBM : DEFAULT_TX_POWER_DBM;

  return stored;
}

int8_t txPowerDbmToQuarterDbm(int8_t dbm) {
  // {actual dBm, value passed to esp_wifi_set_max_tx_power}. The radio only
  // implements these rungs; anything in between is rounded down by the SDK.
  static const struct {
    int8_t dbm;
    int8_t quarter_dbm;
  } ladder[] = {
    { 2,  8}, { 5, 20}, { 7, 28}, { 8, 34}, {11, 44}, {13, 52},
    {14, 56}, {15, 60}, {16, 66}, {18, 72}, {20, 80}
  };

  const uint8_t rungs = (uint8_t)(sizeof(ladder) / sizeof(ladder[0]));

  // Below the floor there is nowhere to go but the floor.
  int8_t selected = ladder[0].quarter_dbm;

  for (uint8_t i = 0; i < rungs; i++) {
    if (dbm >= ladder[i].dbm)
      selected = ladder[i].quarter_dbm;
  }

  return selected;
}
