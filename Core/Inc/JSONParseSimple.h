/*
 * JSONParseSimple.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Pietia2
 */

#ifndef INC_JSONPARSESIMPLE_H_
#define INC_JSONPARSESIMPLE_H_

#define ADDITIONAL_SIGN_PARAMETER_NAME 2

void GetParametersValue(const char *InputJSON, const char *JSONParameterNameIn, char *JSONParamaterValueOut, uint8_t DataType);
void GetParametersValueForecast(const char *InputJSON, const char *JSONParameterNameIn, char *JSONParamaterValueOut, uint8_t DataType, uint8_t NextDayNumber,
        const char *HourString);

enum JSONDataType
{
    Int,
    String
};

#endif /* INC_JSONPARSESIMPLE_H_ */
