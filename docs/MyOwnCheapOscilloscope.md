# My Own Cheap Oscilloscope

**Pre-requisite**: [Multi Channel Voltmeter](https://www.notion.so/MultiChannelVoltmeter.md)

## 시작하는 질문

- 여러 채널의 아날로그 전압도 읽어들일 수 있게 되었는데.... 가만히 있어보자... 디지털 신호처리에서도, 디지털 제어에서도 일정한 주기를 전제로 모든 설명을 했었는데.... VADC에서 샘플링 주기를 어떻게 결정할 수 있지?

전기전자 공학도가 가장 기본적으로 사용하는 계측기에도 위와 같은 고민이 반영되어 있습니다.

- 디지털멀티미터는 현재의 값을 보여주는 것을 목적으로 하고,
- 오실로스코프는 일정한 샘플링 주기를 가지고 트리거 조건을 전후해서 파형을 보여주는 것을 목적으로 하고 있습니다.

앞서 구성하였던 Multi channel voltmeter 예제에서는 주기를 전혀 고려하지 않았습니다. 그러나 디지털 신호처리를 하려면, 디지털 제어를 하려면, 샘플링 주기를 고려해야 합니다. 그러므로 ADC 모듈도 샘플링 타임을 고려해서 프로그래밍 할 수 있어야 합니다.

Converter 가 하나의 채널만 변환해야 할 경우에는 이 문제를 좀 쉽게 해결할 수 있습니다. 그러나 여러 채널을 변환해야 하는 경우라면, 그리고 그 채널들의 샘플링 주기가 다르다면 이 문제는 훨씬 복잡해 집니다. VADC는 이 복잡한 문제를 체계적으로 해결할 수 있는 방안을 가지고 있습니다.

## Objectives

- VADC의 Queue 기능과 Auto Scan 기능을 이해하고,
- 우선순위 결정 방법을 활용할 수 있도록 한다.

## Example Description

- 4채널의 아날로그 전압값을 0.5초 주기로 읽어들인다.

## Background

### Arbitration (중재)

- 공학용어 중에, 특히 Real-Time System 에서 자주 등장하는 말
- 의미를 전달하며 한글로 번역하기 어려운 단어
- 사전적 의미, 중재: 누가 무엇 때문에 싸움이 일어나서 중재가 필요하다는 말일까?
    - 통신 버스에 연결된 노드들 사이에서
    - ADC 변환을 기다리는 아날로그 채널들 사이에서
    - CPU의 처리를 기다리는 여러 인터럽트들 사이에서
    - OS의 서비스를 기다리는 여러 Task 들 사이에서
- 중재 방안: 중요한 것 먼저하자 (우선순위)
    - 급하다고, 여기 저기서 요청한다고, 공유된 자원(리소스)를 한순간에 두군데 이상에서 동시에 사용할 수는 없다.
    - 순서를 결정해 주어야 하고,
    - 그 순서를 결정하는 메카니즘을 잘 활용해서 효율을 높여주어야 한다.
- 중요하다는 말은 빨리 처리해야 한다는 말?
    - 일반적으로 중요하다는 말과 빨리 처리해야 한다는 말은 다른 말입니다.
    - Rate-Monotonic: 그러나 일반적으로 주기적으로 일어나는 사건들 사이에서는 빨리 처리해야 하는 일이 중요한 일이 됩니다
    - 좀 더 자세한 사항은 Real-Time System, 혹은 Real-Time OS 관련 교재를 참고하시기 바랍니다.
- VADC를 사용하여 여러 채널의 정보를 주기적으로 읽어 들여야 하는 경우에도
    - 동시에 변환을 요청하는 경우가 발생하고
    - 중재자(Arbiter)는 설계자가 부여한 우선순위에 따라 순차적으로 변환하게 됩니다.

## AURIX - related

### ADC 동작

- Conversion Modes & Request Sources   
ADC의 경우 총 3개의 request source(2개의 gruop request source와 1개의 background request source)에 의해 Conversion 요청을 받고, 크게 3가지 모드로 동작이 가능합니다.
    - Fixed Channel Conversion (single or continuous)   
    하나의 특정한 Channel Source가 하나의 선택가능한 Channel의 Conversion을 한 번 혹은 반복적으로 요청하는 경우
    - Auto Scan Conversion (single or continuous)   
    하나의 Channnel Scan Source(Request Source 1 혹은 2)가 모든 가용 Channel을 순차적으로 Auto Scan Conversion을 요청하는 경우
    - Channel Sequence Conversion (single or continuous)   
    하나의 Queued Source(Request Source 0)가 8개의 임의의 채널을 순차적으로 요청하는 경우

참고로 Request Source에 의해서 서로 다른 Conversion 모드들이 동시에 진행될 수 있습니다.

- Request Source Control   
모든 Request Source들이 동시에 일어날 때, 'Arbiter'가 다수의 request source로부터 동시에 ADC 요청이 왔을 때 중재자 역할을 합니다.
- Input channel selection
    - Multiplexer가 여러 개의 아날로그 입력(CH0 - CH7) 중 하나를 선택합니다.
    - 세 가지 소스들은 (Request source 0,1,2) linear sequence, arbitrary sequence, 또는 specific channel 중 선택 가능합니다.   
    [참고] pin 개수의 한계 때문에 모든 input channel이 사용 가능하지는 않습니다.
- Conversion control
    - 선택된 아날로그 입력은 conversion control 설정 값(i.e. sample phase duration, result resolution)에 따라 변환되게 됩니다.
    - Conversion parameter는 4개의 input classes (2 group-specific classes, 2 global classes)에 설정됩니다.
    - 각각의 input channel들은 4개의 input classes 중 하나로 개별적 할당이 가능합니다.
    - 각각의 channel들은 standard reference voltage(5V)와 alternate reference voltage 중 하나로 선택이 가능합니다.
- Analog/Digital Converter   
선택된 Input Channel에서 전압값을 받아서 Digital value(8-bit, 10-bit, 12-bit resolution)로 변환시킵니다.
- Result handling   
    - 변환 결과값은 16개의 group-specific result register 중 하나에 저장되고, 1개의 global result register에 저장됩니다.
    - 1개의 Result register는 다수의 channel 그룹에 할당될 수도 있고, 단일 channel에만 할당될 수 도 있습니다.
- Service request generation    
총 3가지 Event에 의해 CPU 혹은 DMA에 Service Request(인터럽트)를 요청할 수 있습니다.
    - Source Event : Request Source에서 하나의 Conversion Sequence가 완료 되었음을 의미하며, 새로운 sequnce를 요청하는 데 사용할 수 있습니다.
    - Channel Event : 특정 Channel의 Conversion이 완료되었음을 의미하며, Limit Check와 같이 이용하여 변환값이 특정 범위를 넘어갔을 때 인터럽트를 발생시키는 용도로 사용할 수 있습니다.
    - Result Event : 새로운 result data를 받을 준비가 되었음을 나타냅니다.

    ![Vadc Conversion Request Unit](docs/Images/MyOwnCheapOscilloscope/Vadc_Conv_Request_Unit.png)

### Modules for ADC

- **Conversion request generation**   
총 3개의 Conversion Request Source(2개의 Group-Specific Source와 1개의 Background Source)에서 Conversion 요청을 생성하며, Arbiter에서 동시에 온 요청 중에서 먼저 Conversion을 수행할 channel을 선택합니다.

    Trigger Event에는 Conversion Request Source에서 하나의 특정 channel을 Conversion 요청하거나, a sequence of channels를 Conversion 요청합니다.

    - Triggers: Software triggers과 External triggers(trigeer pulse; PWM, port pin)로 나뉩니다.

    이러한 trigger의 type과 source, Conversion을 수행할 Channel, Request Source 우선순위는 Application Software에서 설정하며, Request Source는 외부 trigger없이 Software로 activate될 수 있습니다.

    - Operation modes
        - Single-shot mode : trigger가 발생하면 1번만 conversion을 요청하며, 다음 conversion을 요청하기 위해서는 또 1번의 trigger가 필요합니다.
        - Continuous mode : 1번 trigger가 발생하면, program된 conversion (sequence)이 반복적으로 요청됩니다.
    - Types of request sources
        - Queued source: 입력 채널을 임의 순서로 변환하는 방식으로, 입력 채널 수 또는 순서는 자유롭게 변경 가능합니다(한 채널을 여러번 스캔하도록 설정도 가능합니다).
        채널 설정에 따라 매우 짧은 변환도 가능합니다. 스캔 순서는 queue buffer로 설정합니다.
        Request Source 0 → Group-Specific 8-stage Queued Source
        - Channel scan source: 입력 채널과 동일한 순서로 순차적으로 변환을 수행하는 방식으로, Enable된 Channel 중 숫자가 높은 것부터 낮은 것 순으로 Scan합니다.
        Request source 1 → a group-specific channel scan source.
        Request source 2 → a global channel scan source (background source), The background source can request conversions of all channels of all groups.
- **Request source arbitration**
    - Arbiter operation[여기부터 다시 볼 것]
        - Arbiter는 다수의 request source로부터 동시에 ADC 요청이 왔을 때 중재자 역할을 합니다.
        - 주기적으로 request source들의 우선순위를 비교하여 conversion할 source를 결정하는 Polling 방식으로 중재를 하며 중재 구간을 arbitration round라고 합니다.
        - Arbitration round는 다수 개의 arbitration slot으로 구성되어 있습니다.
        - 하나의 Arbitration slot에는 request source가 할당됩니다.
        - 사용자는 Arbitration slot은 duration을 설정할 수 있습니다.
        - 사용자는 각각의 request source의 우선순위를 정할 수 있습니다.
        - 아래 그림은 4개의 arbitration slot을 갖는 arbitration round를 나타냅니다.
        ![VADC Arbiter round](https://aurixtutorial.readthedocs.io/ko/latest/images/MyOwnCheapOscilloscope_ArbitrationRoundWith4ArbitrationSlots.png)