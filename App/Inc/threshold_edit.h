#ifndef __THRESHOLD_EDIT_H
#define __THRESHOLD_EDIT_H

#include <stdint.h>
#include "eeprom_cfg.h"
#include "button.h"


uint8_t Threshold_Edit_Process(Thresholds_t *thresholds);
uint8_t Threshold_Edit_Handle_Button(Thresholds_t *thresholds, Button_Id id);
uint8_t Threshold_Edit_Has_Message(void);


uint8_t Threshold_Edit_Is_Active(void);

#endif
