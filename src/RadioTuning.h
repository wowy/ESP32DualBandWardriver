#pragma once

#include <stdint.h>

#include "configs.h"

// Delay a node waits inside its rendezvous window before transmitting, so
// that nodes sharing the ESP-NOW channel spread their traffic out instead of
// all keying up at the same instant. Returns 0 for a node_count of 0 or 1,
// which is also what an unplaced node reports before core assigns it - such a
// node therefore shares slot 0 with assigned node 0 until its first ADMIN.
uint32_t calculateNodeStaggerOffsetMs(uint8_t node_index,
                                      uint8_t node_count,
                                      uint32_t window_ms);

// Convert a desired maximum TX power in dBm to the 0.25 dBm units taken by
// esp_wifi_set_max_tx_power. The parameter accepts [8, 84], but the radio only
// implements a fixed ladder, so the returned values span 8 (2 dBm) to 80
// (20 dBm) and always round down rather than overshoot a requested cap.
int8_t txPowerDbmToQuarterDbm(int8_t dbm);

// Resolve the stored tx_dbm setting into the power to actually apply. Anything
// outside the supported range - including TX_POWER_AUTO and the 1 that older
// settings files auto-created - means the user has not chosen, so the default
// for the role applies. Solo has no neighbours to desense and keeps its range.
int8_t resolveTxPowerDbm(int8_t stored, bool is_solo);
