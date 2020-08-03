
# Multi Channel Voltmeter
## 시작하는 질문
ADC 라는 것은 알겠는데, AURIX에는 VADC라고 되어 있네? Versatile 이라고? 이것은 또 뭐냐?
아날로그 신호를 디지탈 값으로 바꿔주는 장치를 ADC(Analog-to-Digital Converter)라고 부르고 대부분의 마이크로컨트롤러들은 이 모듈을 가지고 있습니다. ADC를 이야기 할 때 채널의 갯수, 변환속도, 그리고 분해능 등이 중요한 스펙(Specification) 입니다. 아날로그 변환값을 CPU에서 사용하기 위해서는 다음의 몇가지 사항에 대해서 프로그래밍을 해줘야 합니다.

* 변환한 값은 어느 곳에 어떻게 보관할 것인지?
* 언제 샘플링 할 것인지?

ADC 하드웨어가 이 두 가지 사항에 대해서 어떻게 지원해 주는지에 따라 소프트웨어는 확연하게 다른 방식으로 구성되어 집니다. 하드웨어에서 이 사항을 충실하게 지원해 주면 ADC 모듈의 설정에 관심을 가지고 세심하게 프로그래밍 해야 하고, 그렇지 않다면 설정은 간단하지만 샘플링 마다 관련된 동작을 반복적으로 실행해야만 합니다.

AURIX의 VADC는 위의 두가지 사항을 충실하게 지원해 주고 있습니다. 그래서 그냥 ADC라고 부르지 않고 다재다능한(Versatile) ADC라고 부르는 것입니다. VADC를 사용하면, 설정할 때는 심사숙고해야 하지만, 사용할 때는 일반 변수를 읽어들이는 것처럼 쉽게 처리할 수 있습니다.

## Objectives
- VADC의 기본 구조를 이해하고,
- VADC의 내부 블럭별 설정 방법을 이해하고,
- Background Scan 모드로 동작시키는 방법을 익힌다.

## References
- TC23x TC22x Family User's Manual v1.1 - Chap27 VADC
- TC27xD-step User's Manual v2.2 - Chap28 VADC
- iLLD_TC23A_1_0_1_8_0 - Modules/iLLD/VADC

## Example Description
- AN0 와 AN1 의 아날로그 전압을 디지탈로 변환하여 읽어들인다.
  - 두채널의 아날로그 전압값을 전압계로 읽어들이는 것과 마찬가지로 동작한다.

## Background Information
- Analog to digital conveter
  - ADC는 아날로그 측정 값을 디지털 값으로 출력하는 전자 회로
  - 아날로그 입력신호는 오디오, 비디오, 온도 등 매우 종류가 다양
  - 자동차에서 아날로그 값으로 측정하는 센서가 많기 때문에 AURIX에서는
  여러 채널의 아날로그 값을 측정할 수 있는 Multi-Channel 방식을 사용

- Multi channel ADC
  - 여러 채널을 변환하기 위해서 복수개의 ADC를 사용하는 것 보다는,
  한 개의 ADC모듈을 사용하여 analog MUX를 적용하는 것이 더 경제적
  - 대신 ADC는 하나의 모듈이 여러 채널을 제어하기 위해서 복잡한 구조를 갖게 됨

## AURIX -related
### VADC ( Versatile Analog to Digital Converter )
- VADC 구조
  - ​2개의 converter group과 14개의 input channel
  - 각 그룹은 독립적으로 작동하는 ADC kernel
  - 그룹별로 14채널의 전용 아날로그 input multiplexer를 보유
  - 어떤 채널을 어떤 타이밍(sample)에 어떤 우선순위(arbitration)을 갖고 스캔을 할지 제어 가능 (각 기능들의 의미와 설정은 다음장에서)
  - 이번 장에서는 background scan을 이용하여 데이터를 받아올 것이다.
![VADC Structure](https://aurixtutorial.readthedocs.io/ko/latest/images/MultiChannelVoltmeter_StructureOverview.png)

- Backgound scan
  - 가장 낮은 우선순위를 지님
  - 모든 그룹의 모든 채널에 scan 권한이 있음
  - 채널별 할당 된 핀으로 부터 아날로그 voltage 값을 취득
  - 다른 명령없이도 스캔한 값을 지속적으로 디지털 값으로 변환
  - 각각 채널은 독립적으로 동작한다.
  - 사용자가 컨버팅된 값을 이용하기 위해선 그 result가 어떻게 처리되는지 이해할 필요가 있다.

- Result handling
  - 여러 입력을 동시에 받기 때문에 체계적인 처리과정 없이는 데이터 손실이 발생할 수 있음
  - 각 채널에 병렬적으로 사용 가능한 16개의 result register와 1개의 global register가 존재하며,
  - Wait-for-read mode를 사용하여 overwrite에 의한 데이터 손실을 방지한다.   
  [참고] Wait-for-read mode란? - Target result register가 read 가능한 상태가 될 때까지 컨버팅을 정지시키는 모드
  ![ADC Kernel](https://aurixtutorial.readthedocs.io/ko/latest/images/MultiChannelVoltmeter_KernelDiagram.png)

## iLLD - related
### Module Configuration
- VADC 모듈은 작은 모듈들의 집합: 구조적인 사고로 접근할 필요가 있다.
- 상위단에서 하위단까지 단계별 설정이 필요
- ADC configuration
- Group configuration
- Channel configuration
- 설정은 개별적인 구조체와 계측적인 명명법을 사용한 method로 구분하여 구현되어 있다.   
[참고] Demo 코드에서는 채널 설정 부분이 설정영역이 아니라 실행영역에 프로그래밍 되어 있다.

- 실행하면서 채널의 설정을 바꿔야 하는 경우 demo 코드처럼 실행하는 것이 맞으나,
- 대부분의 제어시스템에서는 초기화 단계에서 모듈과 함께 실행하는 것이 옳다고 판단되어,
- 다음과 같이 초기화 단계에서 실행되도록 수정하였다.

```c
//in VadcBackgroundScanDemo.c
void VadcBackgroundScanDemo_init(void)
{
    // ADC module configuration 생성
    IfxVadc_Adc_Config adcConfig;
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);
    IfxVadc_Adc_initModule(&g_VadcBackgroundScan.vadc, &adcConfig);

    // ADC module 초기화
    IfxVadc_Adc_GroupConfig adcGroupConfig;
    IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &g_VadcBackgroundScan.vadc);

    // 사용할 Group 0에 관련된 세부 설정 세팅
    adcGroupConfig.groupId = IfxVadc_GroupId_0;
    adcGroupConfig.master  = adcGroupConfig.groupId;

    // enable background scan source
    adcGroupConfig.arbiter.requestSlotBackgroundScanEnabled = TRUE;

    // enable background auto scan
    adcGroupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;

    // enable all gates in "always" mode (no edge detection)
    adcGroupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;

    // 변경된 설정을 적용하기 위해 다시 초기화
    IfxVadc_Adc_initGroup(&g_VadcBackgroundScan.adcGroup, &adcGroupConfig);

    // Background scan에 2채널을 추가
    uint32                    chnIx;

    // create channel config
    IfxVadc_Adc_ChannelConfig adcChannelConfig[2];

    for (chnIx = 0; chnIx < 2; ++chnIx)
    {
       IfxVadc_Adc_initChannelConfig(&adcChannelConfig[chnIx], &g_VadcBackgroundScan.adcGroup);

       adcChannelConfig[chnIx].channelId         = (IfxVadc_ChannelId)(0 + chnIx);
       adcChannelConfig[chnIx].resultRegister    = (IfxVadc_ChannelResult)(0 + chnIx); // use register #0 and 1 for results
       adcChannelConfig[chnIx].backgroundChannel = TRUE;

       // 채널 초기화
       IfxVadc_Adc_initChannel(&adcChannel[chnIx], &adcChannelConfig[chnIx]);

       // background scan에 추가
       unsigned channels = (1 << adcChannelConfig[chnIx].channelId);
       unsigned mask     = channels;
       IfxVadc_Adc_setBackgroundScan(&g_VadcBackgroundScan.vadc, &g_VadcBackgroundScan.adcGroup, channels, mask);
    }

    // start scan
    IfxVadc_Adc_startBackgroundScan(&g_VadcBackgroundScan.vadc);
    }
```
