# Config Timer for STM32F407
- I use timer 6 to config.
- Clock for timer is APB1 64MHz.
- Prescaler is 63 and ARR (2^16-1).
- Create two function delay follow us and ms.
- Using GPIO (in LedBlink project) to test delay function.
