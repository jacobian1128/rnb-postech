// IntanDAQ.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <fstream>
#include "rhd2000evalboard.h"
#include "rhd2000datablock.h"
#include "WinConsoleCtrl.h"

Rhd2000EvalBoard *evalBoard = new Rhd2000EvalBoard;
int main()
{
	ofstream fileExport("data.bin",ios::binary);
	int errorCode = 0;


	errorCode = evalBoard->open();
	if (errorCode != 1) cout << "failed to open" << endl;
	
	errorCode = evalBoard->uploadFpgaBitfile("main.bit");

	// Default amplifier bandwidth settings
	double desiredLowerBandwidth = 5;
	double desiredUpperBandwidth = 400.0;
	double desiredDspCutoffFreq = 1.0;
	bool dspEnabled = true;
	
	int	evalBoardMode = 0;

	//

	int register59Value;
	int numChannelsOnPort[4] = { 0, 0, 0, 0 };

	Rhd2000EvalBoard::BoardDataSource initStreamPorts[8] = {
		Rhd2000EvalBoard::PortA1,
		Rhd2000EvalBoard::PortA2,
		Rhd2000EvalBoard::PortB1,
		Rhd2000EvalBoard::PortB2,
		Rhd2000EvalBoard::PortC1,
		Rhd2000EvalBoard::PortC2,
		Rhd2000EvalBoard::PortD1,
		Rhd2000EvalBoard::PortD2 };

	Rhd2000EvalBoard::BoardDataSource initStreamDdrPorts[8] = {
		Rhd2000EvalBoard::PortA1Ddr,
		Rhd2000EvalBoard::PortA2Ddr,
		Rhd2000EvalBoard::PortB1Ddr,
		Rhd2000EvalBoard::PortB2Ddr,
		Rhd2000EvalBoard::PortC1Ddr,
		Rhd2000EvalBoard::PortC2Ddr,
		Rhd2000EvalBoard::PortD1Ddr,
		Rhd2000EvalBoard::PortD2Ddr };

	// Enable all data streams, and set sources to cover one or two chips
	// on Ports A-D.
	evalBoard->setDataSource(0, initStreamPorts[0]);
	evalBoard->setDataSource(1, initStreamPorts[1]);

	evalBoard->enableDataStream(0, true);
	evalBoard->enableDataStream(1, true);

	evalBoard->selectAuxCommandBank(Rhd2000EvalBoard::PortA, Rhd2000EvalBoard::AuxCmd3, 0);

	//

	Rhd2000EvalBoard::AmplifierSampleRate sampleRate = Rhd2000EvalBoard::SampleRate20000Hz;
	evalBoard->initialize();
	evalBoard->setSampleRate(sampleRate);
	int numUsbBlocksToRead = 12;

	evalBoard->setMaxTimeStep(100);
	evalBoard->setContinuousRunMode(true);

	// Start SPI interface.
	evalBoard->run();


	int numDataStream = evalBoard->getNumEnabledDataStreams();
	queue<Rhd2000DataBlock> dataQueue;

	COORD posSignal = GetCCursorPos();
	int iter = 0;
	while (evalBoard->isRunning() && iter < 100) {
		errorCode = evalBoard->readDataBlocks(numUsbBlocksToRead, dataQueue);
		if (errorCode == true) {
			iter++;
		}

	}

	evalBoard->resetBoard();


	while (dataQueue.size() > 1) {
		PrintfXY(posSignal.X, posSignal.Y,
			"iter %2d stream %2d %6d\n",
			iter, dataQueue.back().amplifierData.size(),
			dataQueue.back().timeStamp[SAMPLES_PER_DATA_BLOCK - 1]);

		for (int stream = 0; stream < numDataStream; stream++) {
			for (int t = 0; t < SAMPLES_PER_DATA_BLOCK; t++) {
				fileExport.write((char*)&dataQueue.front().timeStamp[t], sizeof(int));
				for (int channel = 0; channel < 32; channel++) {
					fileExport.write((char*)&dataQueue.front().amplifierData[stream][channel][t], sizeof(int));
				}
			}
			dataQueue.pop();
		}
	}
	fileExport.close();
}
