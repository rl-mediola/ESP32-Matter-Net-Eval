#include "ble.hpp"
#include "esp_log.h"
#include <cstdlib>
#include <platform/internal/CHIPDeviceLayerInternal.h>

#include "host/ble_hs.h"
#include "host/ble_hs_pvcy.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include <platform/internal/BLEManager.h>

namespace mediola {
static const char TAG[] = "mediola";

/* Service access callback */
static int gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt * ctxt, void * arg);

/* Service UUID */
static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12, 0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59);

/* A characteristic that can be subscribed to */
static uint16_t gatt_svr_chr_val_handle;
static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33);

/* A custom descriptor */
static const ble_uuid128_t gatt_svr_dsc_uuid =
    BLE_UUID128_INIT(0x01, 0x01, 0x01, 0x01, 0x12, 0x12, 0x12, 0x12, 0x23, 0x23, 0x23, 0x23, 0x34, 0x34, 0x34, 0x34);

std::vector<struct ble_gatt_svc_def> gatt_svr_svcs = {
    {
        /*** Service ***/
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]) {
                {
                    /*** This characteristic can be subscribed to by writing 0x00 and 0x01 to the CCCD ***/
                    .uuid        = &gatt_svr_chr_uuid.u,
                    .access_cb   = gatt_svc_access,
                    .descriptors = (struct ble_gatt_dsc_def[]) { {
                                                                     .uuid      = &gatt_svr_dsc_uuid.u,
                                                                     .att_flags = BLE_ATT_F_READ,
                                                                     .access_cb = gatt_svc_access,
                                                                 },
                                                                 {
                                                                     /* No more descriptors in this characteristic */
                                                                 } },
                    .flags       = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
                    .val_handle  = &gatt_svr_chr_val_handle,
                },
                {
                    /* No more characteristics in this service. */
                } },
    },
};

static int gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt * ctxt, void * arg)
{
    ESP_LOGI(TAG, "%s", __func__);
    if (attr_handle != gatt_svr_chr_val_handle)
    {
        ESP_LOGW(TAG, "Unknown handle");
    }
    switch (ctxt->op)
    {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        ESP_LOGI(TAG, "read characteristic");
        {
            auto r   = std::rand();
            auto err = os_mbuf_append(ctxt->om, &r, sizeof(r));
            return err == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        break;
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        ESP_LOGI(TAG, "write characteristic");
        ESP_LOG_BUFFER_HEX(TAG, ctxt->om->om_data, ctxt->om->om_len);
        break;
    case BLE_GATT_ACCESS_OP_READ_DSC:
        ESP_LOGI(TAG, "read descriptor");
        break;
    case BLE_GATT_ACCESS_OP_WRITE_DSC:
        ESP_LOGI(TAG, "write descriptor");
        break;
    default:
        break;
    }
    return 0;
}

void add_ble_service(void)
{
    auto err = chip::DeviceLayer::Internal::BLEMgrImpl().ConfigureExtraServices(gatt_svr_svcs, true);
    if (CHIP_NO_ERROR != err)
    {
        ESP_LOGE(TAG, "%s failed", __func__);
    }
}

static uint8_t connectable_adv_pattern[] = {
    0x02, 0x01, 0x06, 0x03, 0x03, 0xab, 0xcd, 0x03, 0x03, 0x18, 0x11, 0x12, 0X09, 'n', 'i',
    'm',  'b',  'l',  'e',  '-',  'c',  'o',  'n',  'n',  'e',  't',  'a',  'b',  'l', 'e'
};

/* GAP event handler */
static int ble_multi_adv_gap_event(struct ble_gap_event * event, void * arg)
{
    ESP_LOGI(TAG, "%s event: %d", __func__, event->type);
    switch (event->type)
    {
    case BLE_GAP_EVENT_ADV_COMPLETE:
        break;
    }
    return 0;
}

#ifdef CONFIG_BT_NIMBLE_EXT_ADV
void add_ble_adv(void)
{
    /* Use instance except 0 as Matter advertisement uses instance 0 */
    uint8_t instance = 1;
    struct ble_gap_ext_adv_params params;
    int size_pattern = sizeof(connectable_adv_pattern) / sizeof(connectable_adv_pattern[0]);

    memset(&params, 0, sizeof(params));

    params.connectable   = 1;
    params.scannable     = 1;
    params.legacy_pdu    = 1;
    params.own_addr_type = BLE_OWN_ADDR_RANDOM;
    params.sid           = 1;
    params.primary_phy   = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power      = 127;

    int rc;
    struct os_mbuf * data;
    // int size_pattern = sizeof(legacy_dur_adv_pattern) / sizeof(legacy_dur_adv_pattern[0]);

    if (ble_gap_ext_adv_active(instance))
    {
        ESP_LOGI(TAG, "Instance already advertising");
        return;
    }

    rc = ble_gap_ext_adv_configure(instance, &params, NULL, ble_multi_adv_gap_event, NULL);
    assert(rc == 0);

    /* get mbuf for adv data */
    data = os_msys_get_pkthdr(size_pattern, 0);
    assert(data);

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, connectable_adv_pattern, size_pattern);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(instance, data);
    assert(rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 500, 0);
    assert(rc == 0);
}
#endif /* CONFIG_BT_NIMBLE_EXT_ADV */

} // namespace mediola
