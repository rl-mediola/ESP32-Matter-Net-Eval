#ifndef MEDIOLA_BLE_HPP_INCLUDED
#define MEDIOLA_BLE_HPP_INCLUDED

namespace mediola {

/* should be called before InitServer */
void add_ble_service(void);

#ifdef CONFIG_BT_NIMBLE_EXT_ADV
void add_ble_adv(void);
#endif
} // namespace mediola

#endif /* MEDIOLA_BLE_HPP_INCLUDED */
