#include "hcpbridge.h"

namespace esphome
{
  namespace hcpbridge
  {

    void HCPBridge::setup()
    {
      this->is_connected_->publish_state(false);

      int8_t rx = this->rx_pin_ == nullptr ? PIN_RXD : this->rx_pin_->get_pin();
      int8_t tx = this->tx_pin_ == nullptr ? PIN_TXD : this->tx_pin_->get_pin();
      int8_t rts = this->rts_pin_ == nullptr ? -1 : this->rts_pin_->get_pin();

      this->engine = &HoermannGarageEngine::getInstance();
      this->engine->setup(rx, tx, rts);
    }

    void HCPBridge::loop() {
      // On single-core chips there is no dedicated modbus task (it would starve
      // the main loop), so service modbus cooperatively here. mb.task() is
      // non-blocking. Dual-core chips run the task instead, so this is a no-op.
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32C6)
      this->engine->handleModbus();
#endif
    }

    void HCPBridge::update() {
      if (this->engine->state->valid) {
        if (this->is_connected_->state != true) {
          this->is_connected_->publish_state(true);
        }
      } else {
        if (this->is_connected_->state != false) {
          this->is_connected_->publish_state(false);
        }
      }
    }

  }
}