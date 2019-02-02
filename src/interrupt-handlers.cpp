extern "C" {
void NMI_Handler(void) {}

void HardFault_Handler(void) {}

void MemManage_Handler(void) {}

void BusFault_Handler(void) {}

void UsageFault_Handler(void) {}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void WWDG_IRQHandler(void) {}
void PVD_IRQHandler(void) {}
void TAMPER_STAMP_IRQHandler(void) {}
void RTC_WKUP_IRQHandler(void) {}
void FLASH_IRQHandler(void) {}
}