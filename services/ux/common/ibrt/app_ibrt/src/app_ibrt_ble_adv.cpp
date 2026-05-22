#ifdef __IAG_BLE_INCLUDE__

#include "bluetooth_bt_api.h"
#include "app_ibrt_ble_adv.h"
#include "hal_trace.h"
#include "me_api.h"
#include "cmsis_os.h"
#include "factory_section.h"
#include "bluetooth_ble_api.h"
#include "co_bt_defines.h"
#include "app_tws_ibrt.h"
#include "app_ibrt_debug.h"

SLAVE_BLE_MODE_T slaveBleMode;
static bes_ble_gap_cus_adv_param_t app_ble_adv_para_data_cfg;
#define APP_IBRT_BLE_ADV_DATA_MAX_LEN (31)
#define APP_IBRT_BLE_SCAN_RSP_DATA_MAX_LEN (31)

const char *g_slave_ble_state_str[] =
{
    "BLE_STATE_IDLE",
    "BLE_ADVERTISING",
    "BLE_STARTING_ADV",
    "BLE_STOPPING_ADV",
};
    
const char *g_slave_ble_op_str[] =
{
    "BLE_OP_IDLE",
    "BLE_START_ADV",
    "BLE_STOP_ADV",
    "BLE_STOPPING_ADV",
};

#define SET_SLAVE_BLE_STATE(newState)                                                                                     \
    do                                                                                                              \
    {                                                                                                               \
        LOG_I("[BLE][STATE]%s->%s at line %d", g_slave_ble_state_str[slaveBleMode.state], g_slave_ble_state_str[newState], __LINE__); \
        slaveBleMode.state = (newState);                                                                              \
    } while (0);

#define SET_SLAVE_BLE_OP(newOp)                                                                            \
    do                                                                                               \
    {                                                                                                \
        LOG_I("[BLE][OP]%s->%s at line %d", g_slave_ble_op_str[slaveBleMode.op], g_slave_ble_op_str[newOp], __LINE__); \
        slaveBleMode.op = (newOp);                                                                     \
    } while (0);
/*****************************************************************************
 Prototype    : app_slave_ble_cmd_complete_callback
 Description  : stop ble adv
 Input        : const void *para
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2019/12/28
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_slave_ble_cmd_complete_callback(const uint8_t *para)
{
    slave_ble_cmd_comp_t *p_cmd_complete = (slave_ble_cmd_comp_t *)para;

    switch (p_cmd_complete->cmd_opcode)
    {
        case HCI_BLE_ADV_CMD_OPCODE:
            if(!p_cmd_complete->param[0])
                app_ibrt_ble_switch_activities();
            else
            {
                LOG_I("%s error p_cmd_complete->param[0] %d", __func__, p_cmd_complete->param[0]);
                SET_SLAVE_BLE_STATE(BLE_STATE_IDLE);
                SET_SLAVE_BLE_OP(BLE_OP_IDLE);
            } 
        default:
        break;
    }
}

/*****************************************************************************
 Prototype    : app_ibrt_ble_adv_start
 Description  : enable ble adv
 Input        : uint8_t adv_type, uint16_t advInterval
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2019/12/28
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ble_adv_start(uint8_t adv_type, uint16_t advInterval)
{
    LOG_I("ble adv start with adv_type %d advIntervalms %dms", adv_type, advInterval);
    app_ble_adv_para_data_cfg.adv_type = (BLE_ADV_TYPE_E)adv_type;
    app_ble_adv_para_data_cfg.adv_interval = advInterval;

    switch(slaveBleMode.state)
    {
        case BLE_ADVERTISING:
            SET_SLAVE_BLE_STATE(BLE_STOPPING_ADV);
            SET_SLAVE_BLE_OP(BLE_START_ADV);
            bes_ble_gap_custom_adv_stop(BLE_ADV_ACTIVITY_USER_0);
            break;
        case BLE_STARTING_ADV:
        case BLE_STOPPING_ADV:
            SET_SLAVE_BLE_OP(BLE_START_ADV);
            break;
        case BLE_STATE_IDLE:
            SET_SLAVE_BLE_STATE(BLE_STARTING_ADV);
            bes_ble_gap_custom_adv_write_data(&app_ble_adv_para_data_cfg);
            bes_ble_gap_custom_adv_start(BLE_ADV_ACTIVITY_USER_0);
            break;
        default:
        break;
    }

}
/*****************************************************************************
 Prototype    : app_ibrt_ble_adv_stop
 Description  : disable ble adv
 Input        : None
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2019/12/28
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ble_adv_stop(void)
{
    LOG_I("%s",__func__);
    SET_SLAVE_BLE_STATE(BLE_STOPPING_ADV);
    SET_SLAVE_BLE_OP(BLE_STOP_ADV);
    bes_ble_gap_custom_adv_stop(BLE_ADV_ACTIVITY_USER_0);
}
/*****************************************************************************
 Prototype    : app_ibrt_ble_switch_activities
 Description  : ble adv state machine switch
 Input        : None
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2019/12/28
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ble_switch_activities(void)
{
    switch(slaveBleMode.state)
    {
        case BLE_STARTING_ADV:
            SET_SLAVE_BLE_STATE(BLE_ADVERTISING);
            if(slaveBleMode.op == BLE_START_ADV)
            {
                SET_SLAVE_BLE_OP(BLE_OP_IDLE);
            }
            break;
       case BLE_STOPPING_ADV:
            SET_SLAVE_BLE_STATE(BLE_STATE_IDLE);
            if(slaveBleMode.op == BLE_STOP_ADV)
            {
                SET_SLAVE_BLE_OP(BLE_OP_IDLE);
            }
            break;            
       default:
        break; 
    }
    switch(slaveBleMode.op)
    {
        case BLE_START_ADV:
             app_ibrt_ble_adv_start(ADV_CONN_UNDIR, app_ble_adv_para_data_cfg.adv_interval);
             break;
        default:
             break;
    }
}

/*****************************************************************************
 Prototype    : app_ibrt_ble_adv_data_config
 Description  : config ble adv data and scan response data by customer
 Input        : uint8_t *advData, uint8_t advDataLen
              : uint8_t *scanRspData, uint8_t scanRspDataLen
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2019/12/28
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ble_adv_data_config(uint8_t *advData, uint8_t advDataLen,
                                            uint8_t *scanRspData, uint8_t scanRspDataLen)
{
    ASSERT(APP_IBRT_BLE_ADV_DATA_MAX_LEN >= advDataLen, "ble adv data len exceed");
    ASSERT(APP_IBRT_BLE_SCAN_RSP_DATA_MAX_LEN >= scanRspDataLen, "scan response data len exceed")
    memcpy(app_ble_adv_para_data_cfg.adv_data, advData, advDataLen);
    memcpy(app_ble_adv_para_data_cfg.scan_rsp_data, scanRspData, scanRspDataLen);
    app_ble_adv_para_data_cfg.adv_data_size = advDataLen;
    app_ble_adv_para_data_cfg.scan_rsp_data_size = scanRspDataLen;
}
#endif


