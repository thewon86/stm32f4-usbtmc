# stm32f4-usbtmc
基于 stm32f4 PCD 及 MX 生成设备框架实现的 USBTMC 协议，支持 USB488 协议。支持大数据量的二进制数据请求（例如截屏数据）



### 已支持特性汇总

- [x] 基本的数据收发。
- [x] 大数据量请求，超过 MSP，甚至超过主机单次请求缓存容量；即支持 multiple transfers。
- [x] 支持二进制数据请求。
- [ ] 可以支持 TermChar
- [ ] 可以支持 USB488 Trigger

USBTMC bRequest 支持

- [x] INITIATE_ABORT_BULK_OUT
- [x] CHECK_ABORT_BULK_OUT_STATUS
- [x] INITIATE_ABORT_BULK_IN
- [x] CHECK_ABORT_BULK_IN_STATUS
- [x] INITIATE_CLEAR
- [x] CHECK_CLEAR_STATUS
- [x] GET_CAPABILITIES
- [ ] INDICATOR_PULSE



### 未支持特性汇总

- [ ] All mandatory SCPI commands。
- [ ] SR1 capable。
- [ ] RL1 capable。
- [ ] DT1 capable。
- [ ] READ_STATUS_BYTE
- [ ] REN_CONTROL
- [ ] GO_TO_LOCAL
- [ ] LOCAL_LOCKOUT
