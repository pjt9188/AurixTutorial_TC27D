
## Twinkle Twinkle Little Star
### Example Description
- STM(System Timer)를 사용해서 1초 마다 인터럽트를 발생시킨다.
- 인터럽트를 이용해서 2초 주기로 LED(TC275 : 10.2 Port)을 점멸시킨다.   
(StmDemo 코드 사용)

### 32-bit MCU(microcontroller unit)
- TC2XX는 한 번의 명령어로 최대 32bit까지 데이터를 다룰 수 있다.
- 기본 클럭세팅으로는 CPU0, PLL = 200Mhz / STM = 100Mhz로 세팅되어있다.

### STM(System Timer)
- **Module Description**
  - Stm : STM(System timer)의 init, STM의 interrupt handler로 구성
  - Led : 보드 위의 LED(TC275 : Port 10.2)를 init하거나, on, off, toggle하는 함수로 구성

- **64-bit timer**   
  - 64-bit System Timer(2개의 32bit 레지스터)를 이용하여 Tick을 누적시킨다.   
    [ 1 tick = 1 / F<sub>stm</sub> = 1 / 100Mhz = 10ns ]   
  - Compare Register(32bit)와 System Timer가 같아지면 Interrupt 발생한다.   
    (TC275의 경우 총 3개의 STM이 존재하며 각각의 STM에는 2개의 Compare Register(CMP0, CMP1)이 존재한다.   
    각각의 CMP0와 CMP1이 STM과 같아졌을 때 각각 Service Requset(Interrupt)를 발생시킨다.)

    ![STM Module](https://aurixtutorial.readthedocs.io/ko/latest/images/TwinkleTwinkleLittleStar_GeneralBlockDiagram.png) 
  
### Algorithm
- STM init
- STM interrupt가 일정한 주기로 발생 -> interrupt handler:
  - STM의 Interrupt flag를 reset, <code>IfxStm_clearCompareFlag</code>
  - Compare register의 값을 주기만큼 더해주며, <code>IfxStm_increaseCompare</code>
  - Interrupt를 다시 활성화한다. <code>IfxCpu_enableInterrupts</code>
  - 그 후 LED blink function 실행한다 <code>IfxBlinkLed_Task</code>
---