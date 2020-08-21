/**
 *  @example serial_port_read.cpp
 */

#include "libserial/SerialPort.h"
#include "libserial/SerialPortConstants.h"
#include "libserial/SerialStream.h"

#include <gtest/gtest.h>
#include <mutex>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <type_traits>
#include <unistd.h>

using namespace LibSerial;

constexpr const char* const SERIAL_PORT_FRONT = "/dev/ttyUSB0";
constexpr const char* const SERIAL_PORT_BACK = "/dev/ttyUSB1";

void signal_callback_handler(int signum)
{
	exit(signum);
}


int main()
{
	SerialStream frontSensorStream;
	SerialStream backSensorStream;

	signal(SIGINT, signal_callback_handler);

	std::cout << "program started" << std::endl;

	try
	{
		frontSensorStream.Open(SERIAL_PORT_FRONT);
	}
	catch (const OpenFailed&)
	{
		std::cerr << "The serial port did not open correctly." << std::endl ;
		return EXIT_FAILURE ;
	}

	try
	{
		backSensorStream.Open(SERIAL_PORT_BACK);
	}
	catch (const OpenFailed&)
	{
		std::cerr << "The serial port did not open correctly." << std::endl;
		return EXIT_FAILURE ;
	}

	frontSensorStream.SetBaudRate(BaudRate::BAUD_57600);
	frontSensorStream.SetCharacterSize(CharacterSize::CHAR_SIZE_8);
	frontSensorStream.SetStopBits(StopBits::STOP_BITS_1);
	frontSensorStream.SetParity(Parity::PARITY_NONE);
	frontSensorStream.SetFlowControl(FlowControl::FLOW_CONTROL_NONE);

	backSensorStream.SetBaudRate(BaudRate::BAUD_57600);
	backSensorStream.SetCharacterSize(CharacterSize::CHAR_SIZE_8);
	backSensorStream.SetStopBits(StopBits::STOP_BITS_1);
	backSensorStream.SetParity(Parity::PARITY_NONE);
	backSensorStream.SetFlowControl(FlowControl::FLOW_CONTROL_NONE);

	std::cout << "ports opened, about to start capture" << std::endl;

	frontSensorStream << "random_data" << std::endl;
	backSensorStream << "random_data" << std::endl;

	while(!frontSensorStream.IsDataAvailable() &&
		  !backSensorStream.IsDataAvailable())
	{
		std::cout << "Data is not available." << std::flush;
		usleep(5000);
	}

	while(frontSensorStream.IsDataAvailable() ||
		  backSensorStream.IsDataAvailable())
	{
		std::cout << "Data is available." << std::endl;

		char frontSensorData = '\0';
		char backSensorData = '\0';

		frontSensorStream.get(frontSensorData);
		backSensorStream.get(backSensorData);

		std::cout << "front sensor = " << frontSensorData << std::endl;
		std::cout << "back sensor = " << backSensorData << std::endl;

		usleep(1000);
	}

	frontSensorStream.Close();
	backSensorStream.Close();

	std::cout << "program done" << std::endl;
	return EXIT_SUCCESS;
}
