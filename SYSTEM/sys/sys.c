#include "sys.h"

// 时钟系统配置函数
// Fvco=Fs*(plln/pllm);
// SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
// Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

// Fvco:VCO频率
// SYSCLK:系统时钟频率
// Fusb:USB,SDIO,RNG等的时钟频率
// Fs:PLL输入时钟频率,可以是HSI,HSE等. 
// plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
// pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
// pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
// pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.

// 外部晶振为25M的时候,推荐值:plln=360,pllm=25,pllp=2,pllq=8.
// 得到:Fvco=25*(360/25)=360Mhz
//      SYSCLK=360/2=180Mhz
//      Fusb=360/8=45Mhz
int SystemClock_Config(void)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStructure = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
	__HAL_RCC_CRC_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  // 设置调压器输出电压级别 1
    
    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSE;   // 时钟源为 HSE
    RCC_OscInitStructure.HSEState = RCC_HSE_ON;                     // 打开 HSE
    RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;                 // 打开 PLL
    RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSE;         // PLL 时钟源选择 HSE
    RCC_OscInitStructure.PLL.PLLM = 25;                           // 主 PLL 和音频 PLL 分频系数(PLL 之前的分频),取值范围: 2~63.
    RCC_OscInitStructure.PLL.PLLN = 336;                           // 主 PLL 倍频系数(PLL 倍频),取值范围: 64~432.  
    RCC_OscInitStructure.PLL.PLLP = RCC_PLLP_DIV2;                           // 系统时钟的主 PLL 分频系数(PLL 之后的分频),取值范围: 2,4,6,8.(仅限这4个值!)
    RCC_OscInitStructure.PLL.PLLQ = 7;                           // USB/SDIO/随机数产生器等的主 PLL 分频系数(PLL 之后的分频),取值范围: 2~15.
    ret = HAL_RCC_OscConfig(&RCC_OscInitStructure);
    if(ret != HAL_OK) return -1;

    ret = HAL_PWREx_EnableOverDrive(); // 开启 Over-Driver 功能
    if(ret != HAL_OK) return -1;

    // 选中 PLL 作为系统时钟源并且配置 HCLK, PCLK1 和 PCLK2
    RCC_ClkInitStructure.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;        // 设置系统时钟时钟源为 PLL
    RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;               // AHB 分频系数为  1
    RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV4;                // APB1 分频系数为 4
    RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV2;                // APB2 分频系数为 2
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_5);  // 同时设置 FLASH 延时周期为 5WS，也就是 6 个 CPU 周期。
    if(ret != HAL_OK) return -1;

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 144;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
    ret = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    if(ret != HAL_OK) return -1;

    return 0;
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(uint32_t addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
