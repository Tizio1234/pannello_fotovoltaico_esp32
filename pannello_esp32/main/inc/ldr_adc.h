#ifndef _LDR_ADC_H_
#define _LDR_ADC_H_

typedef struct
{
    int raw;
    int voltage;
} adc_reading_t;


void adc_init(void);
int x_adc_read(adc_reading_t* adc_reading);
int y_adc_read(adc_reading_t* adc_reading);
void adc_deinit(void);

#endif