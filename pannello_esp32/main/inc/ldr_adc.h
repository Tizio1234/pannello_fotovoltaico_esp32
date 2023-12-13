#ifndef _LDR_ADC_H_
#define _LDR_ADC_H_

//#define Y_ADC

typedef struct
{
    int raw;
    int voltage;
} adc_reading_t;

void adc_init(void);
int x_adc_read(adc_reading_t* adc_reading);

#ifdef Y_ADC
int y_adc_read(adc_reading_t* adc_reading);
#endif

void adc_deinit(void);

#endif