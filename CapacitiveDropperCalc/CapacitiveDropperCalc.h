/*
 Name:		CapacitiveDropperCalc.h
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
*/
#ifndef CAPACITIVEDROPPERCALC_H_INCLUDED
#define CAPACITIVEDROPPERCALC_H_INCLUDED

#define settingsName "settings"
#define defaultFreq 50
#define defaultVoltage 230
#define settingsSize 64

void clearInput(void);
int settings(void);
int checkIfSettingsFileExist(std::string name);
int createSettingsFile(std::string name, short freq, short voltage);
std::pair<short, short> readSettingsFile(std::string name);
int calculations(void);
void findClosestValues(double valueToCompare);
void printResult(char range, double calculatedCapacitance);

#endif // CAPACITIVEDROPPERCALC_H_INCLUDED