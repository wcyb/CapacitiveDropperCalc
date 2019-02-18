/*
 Name:		CapacitiveDropperCalc.cpp
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
*/
#include "pch.h"
#include "eSeries.h"
#include "CapacitiveDropperCalc.h"

int main()
{
	short selectedOption = 0;

	std::cout.setf(std::ios::fixed);
	std::cout << "Capacitive dropper calculator v1.0" << std::endl;
	std::cout << "(c) 2016-2019 Wojciech Cybowski. All rights reserved." << std::endl;
	while (true)
	{
		std::cout << "Select option:" << std::endl << "1. Calculate" << std::endl << "2. Settings" << std::endl << "3. Exit" << std::endl;
		std::cin >> selectedOption; //read one char and check if valid
		switch (selectedOption)
		{
		case 1://calculations
			calculations();
			break;
		case 2://settings
			settings();
			break;
		case 3://exit
			return 0;
		default:
			std::cout << "No such option." << std::endl;
		}
		clearInput();
	}
	return 0;
}

void clearInput()
{
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int checkIfSettingsFileExist(std::string name)
{
	std::fstream settingsFile(name, std::ios::in);
	if (settingsFile.fail()) //if settings file don't exist then create new
	{
		settingsFile.close();
		std::cout << "Settings file not exist, creating new file with default parameters." << std::endl;
		return createSettingsFile(name, defaultFreq, defaultVoltage);
	}
	return 1;
}

int createSettingsFile(std::string name, short freq, short voltage)
{
	std::fstream settingsFile(name, std::ios::out);
	if (settingsFile.fail())
	{
		settingsFile.close();
		std::cout << "Cannot create settings file. Exiting..." << std::endl;
		return 0;
	}
	settingsFile << "#AC frequency[Hz], AC voltage[V]\n;" << freq << ";" << voltage << ";";
	settingsFile.close();
	return 1;
}

std::pair<short, short> readSettingsFile(std::string name)
{
	char settingsContent[settingsSize] = { 0 };
	short acFreq, acVoltage;

	std::fstream settingsFile(name, std::ios::in);
	settingsFile.read(settingsContent, settingsSize);
	settingsFile.close();

	if (sscanf_s(settingsContent, "%*3s %*14s %*2s %*10s ;%hu;%hu;", &acFreq, &acVoltage) != 2)//read loaded settings values
	{//if file was not as expected, then create default settings file
		if (!createSettingsFile(name, defaultFreq, defaultVoltage)) return std::pair<short, short>(0, 0);//if settings file can't be created, then return zeros
		return std::pair<short, short>(defaultFreq, defaultVoltage);//else return defaults
	}
	return std::pair<short, short>(acFreq, acVoltage);
}

int settings()
{
	short acFreq, acVoltage;
	short newAcVoltage, newAcFreq;
	short selectedOption = 0;
	bool settingsChanged = false;

	//check if settings file exist and if not, try to create default
	if (!checkIfSettingsFileExist(settingsName))
		if (!createSettingsFile(settingsName, defaultFreq, defaultVoltage)) return 0;//if we can't create settings file then exit

	std::pair<short, short> readedSettings = readSettingsFile(settingsName);
	acFreq = readedSettings.first;
	acVoltage = readedSettings.second;

	while (true)
	{
		std::cout << "Current settings:" << std::endl << "AC Frequency: " << acFreq << "[Hz]" << std::endl << "AC Voltage: " << acVoltage << "[V]" << std::endl;
		std::cout << "Which setting you want to change:" << std::endl << "1. AC Frequency" << std::endl << "2. AC Voltage" << std::endl << "3. None, return" << std::endl;
		std::cin >> selectedOption;

		switch (selectedOption)
		{
		case 1://acfreq
			std::cout << "Enter new frequency value [Hz]: ";
			std::cin >> newAcFreq;
			if (newAcFreq <= 0)
			{
				std::cout << "Wrong frequency value." << std::endl;
				break;
			}
			acFreq = newAcFreq;
			settingsChanged = true;
			break;

		case 2://acvolts
			std::cout << "Enter new voltage value [V]: ";
			std::cin >> newAcVoltage;
			if (newAcVoltage <= 0)
			{
				std::cout << "Wrong voltage value." << std::endl;
				break;
			}
			acVoltage = newAcVoltage;
			settingsChanged = true;
			break;

		case 3://exit
			if (settingsChanged)
			{
				std::cout << "Saving changes..." << std::endl;
				return createSettingsFile(settingsName, acFreq, acVoltage);
			}
			return 1;

		default:
			std::cout << "No such option." << std::endl;
		}
		clearInput();
	}
	return 0;
}

int calculations()
{
	int acFreq, acVoltage;
	double calculatedCapacitance = 0.0;
	float outputVoltage = 0.0f;
	float outputCurrent = 0.0f;
	float neededReactance = 0.0f;

	if (!checkIfSettingsFileExist(settingsName))
	{
		std::cout << "Settings file does not exist. Go to settings to create settings file." << std::endl;
		return 0;
	}

	std::pair<short, short> readedSettings = readSettingsFile(settingsName);
	acFreq = readedSettings.first;
	acVoltage = readedSettings.second;

	std::cout << "Enter required output voltage [V]: ";
	std::cin >> outputVoltage;
	if (outputVoltage >= acVoltage || outputVoltage <= 0.0f)
	{
		std::cout << "Wrong voltage value. Use dot as decimal point." << std::endl;
		return 0;
	}

	std::cout << "Enter required output current [A]: ";
	std::cin >> outputCurrent;
	if (outputCurrent <= 0.0f)
	{
		std::cout << "Wrong current value. Use dot as decimal point." << std::endl;
		return 0;
	}

	neededReactance = (acVoltage - outputVoltage) / outputCurrent;
	calculatedCapacitance = 1.0 / (2 * M_PI * acFreq * neededReactance); //capacitance is in farads
	std::cout << std::endl << "(higher capacitance = higher current)" << std::endl;

	if (calculatedCapacitance < 1.0 && calculatedCapacitance >= 10e-4) //for mF
	{
		calculatedCapacitance = calculatedCapacitance * 1000;
		printResult('m', calculatedCapacitance);
	}
	else if (calculatedCapacitance < 10e-4 && calculatedCapacitance >= 10e-7) //uF
	{
		calculatedCapacitance = calculatedCapacitance * 10e5;
		printResult('u', calculatedCapacitance);
	}
	else if (calculatedCapacitance < 10e-7 && calculatedCapacitance >= 10e-10) //nF
	{
		calculatedCapacitance = calculatedCapacitance * 10e8;
		printResult('n', calculatedCapacitance);
	}
	else if (calculatedCapacitance < 10e-10 && calculatedCapacitance >= 10e-13) //pF
	{
		calculatedCapacitance = calculatedCapacitance * 10e11;
		printResult('p', calculatedCapacitance);
	}
	else
	{
		std::cout << "Needed capacitance is: " << std::setprecision(1) << calculatedCapacitance << "[F]" << std::endl << std::endl;
	}
	return 0;
}

void printResult(char range, double calculatedCapacitance)
{
	char lowerRange;

	switch (range)
	{
	case 'm':
		lowerRange = 'u';
		break;
	case 'u':
		lowerRange = 'n';
		break;
	case 'n':
		lowerRange = 'p';
		break;
	case 'p':
		lowerRange = 'f';
		break;
	default:
		lowerRange = ' ';
		break;
	}

	std::cout << "Needed capacitance is: " << std::setprecision(2)
		<< calculatedCapacitance << "[" << range << "F]" << std::endl << std::endl;
	std::cout << "For one capacitor:" << std::endl;
	std::cout << "All values are in [" << range << "F]:" << std::endl;
	findClosestValues(calculatedCapacitance);
	if (range == 'p') return;
	if ((calculatedCapacitance / 2) >= 1.0)
	{
		std::cout << "For two capacitors (" << std::setprecision(2)
			<< (calculatedCapacitance / 2) << "[" << range << "F] each):" << std::endl;
		std::cout << "All values are in [" << range << "F]:" << std::endl;
		findClosestValues(calculatedCapacitance / 2);
	}
	else
	{
		//calculatedCapacitance * 1000 to get two lower values from one higher value
		std::cout << "For two capacitors (" << std::setprecision(2)
			<< ((calculatedCapacitance * 1000) / 2) << "[" << lowerRange << "F] each):" << std::endl;
		std::cout << "All values are in [" << lowerRange << "F]:" << std::endl;
		findClosestValues((calculatedCapacitance * 1000) / 2);
	}
}

void findClosestValues(double valueToCompare)
{
	float divider = 1.0f;
	double closestValue = std::numeric_limits<double>::max();
	double lastClosestValue = std::numeric_limits<double>::max();
	short closestValuePosition = 0;
	short lastClosestValuePosition = 0;

	short valueSeries = 6; //starting val series
	short seriesTolerance = 20;
	short const* currentSeriesArray = nullptr;

	if (valueToCompare < 10.0)//divide by 100 to change scale of values, to change ranges only divider needs to change
	{
		divider = 0.01f;
	}
	else if (valueToCompare < 100.0)
	{
		divider = 0.1f;
	}

	while (valueSeries <= 48)//or <= 96 if needed
	{
		switch (valueSeries)
		{
		case 6:
			currentSeriesArray = e6;
			break;
		case 12:
			currentSeriesArray = e12;
			break;
		case 24:
			currentSeriesArray = e24;
			break;
		case 48:
			currentSeriesArray = e48;
			break;
			/*case 96:
				currentSeriesArray = e96;
				break;*/
		}

		for (int i = 0; i < valueSeries; i++)
		{
			double tmp = std::abs((static_cast<double>(currentSeriesArray[i]) * divider) - valueToCompare); //get difference between value from array and calculated

			if (tmp < closestValue)
			{
				lastClosestValue = closestValue; //save latest closest value
				closestValue = tmp; //save new closest value
				lastClosestValuePosition = closestValuePosition; //save position of latest closest value in array
				closestValuePosition = i; //save position of new closest value in array
			}
		}
		std::cout << "E" << valueSeries << "(+-" << seriesTolerance << "%) first closest value : " << std::setprecision(2)
			<< (currentSeriesArray[closestValuePosition] * divider) << std::endl << "(" << std::setprecision(1)
			<< ((closestValue / valueToCompare) * 100) << "% difference from calculated value)" << std::endl;
		if (((lastClosestValue / valueToCompare) * 100) < 50)//show second value only if difference is less than 50%
		{
			std::cout << "E" << valueSeries << "(+-" << seriesTolerance << "%) second closest value: " << std::setprecision(2)
				<< (currentSeriesArray[lastClosestValuePosition] * divider) << std::endl << "(" << std::setprecision(1)
				<< ((lastClosestValue / valueToCompare) * 100) << "% difference from calculated value)" << std::endl << std::endl;
		}
		else
		{
			std::cout << std::endl;
		}

		valueSeries *= 2;
		seriesTolerance /= 2;
		closestValue = std::numeric_limits<double>::max();
		lastClosestValue = std::numeric_limits<double>::max();
		closestValuePosition = 0;
		lastClosestValuePosition = 0;//reset vars
	}
}