//SIMULATE.H
#pragma once
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include "Cluster.h"

int cellDistances[9][9] = {
	0, 2000, 2000, 4000, 6000, 4000, 4000, 6000, 6000,
	2000, 0, 2000, 2000, 4000, 2000, 4000, 4000, 6000,
	2000, 2000, 0, 4000, 4000, 2000, 2000, 4000, 4000,
	4000, 2000, 4000, 0, 2000, 2000, 4000, 4000, 6000,
	6000, 4000, 4000, 2000, 0, 2000, 4000, 2000, 4000,
	4000, 2000, 2000, 2000, 2000, 0, 2000, 4000, 4000,
	4000, 4000, 2000, 4000, 4000, 2000, 0, 2000, 2000,
	6000, 4000, 4000, 4000, 2000, 2000, 2000, 0, 2000,
	6000, 6000, 4000, 6000, 4000, 4000, 2000, 2000, 0
};

//Prioritize traffic with a lower time.
class CallComparator {
public:
	bool operator() (Call& left, Call& right) {
		return left.getTime() > right.getTime();
	}
};

class Simulate {
	int accepted = 0, blocked = 0, totalCalls = 0, time;
	double averageSIR = 0;
	Cluster clusterOne, clusterTwo, clusterThree;
	std::array<Cluster, 3> clusters = { clusterOne, clusterTwo, clusterThree };
public:
	std::priority_queue<Call, std::vector<Call>, CallComparator> callQueue;
	Simulate(std::string);
	bool availability(Cluster&, int, Call&);
	bool connecting(Call&);
	bool huntingSequence(Call&);
	double SIR(int, Cluster&, Call&);
	double GOS();
	double avgSIR();
	void open(std::string);
};

Simulate::Simulate(std::string filename) : time(0) {
	//Open the file and fill the call queue.
	open(filename);
	//Call queue is filled. Begin processing.
	std::cout << "#######################\n";
	std::cout << filename << "\n";
	std::cout << "########################\n";
	while (!callQueue.empty()) {
		Call call = callQueue.top();
		//If the line is not connected / disconnected.
		if (connecting(call)) {
			//If the call failed to connect, remove from queue.
			if (!call.connectionStatus()) {
				callQueue.pop();
			}
			//If call succeeded to connect
			else {
				int lastedFor = call.getTime() + call.getDuration();
				time += call.getTime();
				Call nextCall = { call.getID() + 1, lastedFor, call.getCell(),
					call.getChannel() };
				callQueue.push(nextCall);
			}
		}
		//If the call failed to connect during the connection phase, drop it.
		else {
			callQueue.pop();
		}
		totalCalls++;
	}
	//Print final readouts to console.
	std::cout << "Total calls accepted: " << accepted;
	std::cout << " Total calls blocked: " << blocked;
	std::cout << "\nGoS = " << GOS() << "% Average SIR = " << avgSIR();
	std::cout << " dB" << std::endl;
	std::cout << "\n Total calls: " << totalCalls << "\n";
}

void Simulate::open(std::string filename) {
	int id, time, cell, duration;

	std::ifstream file;
	std::string temp;
	file.open(filename);
	//Ignore the first line from the file.
	std::getline(file, temp, '\n');
	//Create Call objects for each subsequent line.
	while (!file.eof()) {
		file >> id;
		file >> time;
		file >> cell;
		file >> duration;
		//Cell - 1 to account for channels beginning at 1 rather than zero.
		Call call = { id, time, cell - 1, duration };
		callQueue.push(call);
	}
	file.close();
}

bool Simulate::connecting(Call& call) {
	std::cout << "Call # = " << call.getID() << ", Cell =  " << call.getCell() + 1
		<< ", Time = " << call.getTime() << ", Duration = " << call.getDuration();
	//Check if the call successfully connected after hunting sequence.
	if (huntingSequence(call)) {
		std::cout << " Accepted\n";
		//Disconnect call after it's accepted.
		call.disconnect();
		//Update amount of accepted calls for GoS.
		accepted++;
		return true;
	}
	else {
		std::cout << " Blocked\n";
		//Set call connected status to false.
		call.disconnect();
		//Update blocked amount of calls for GoS.
		blocked++;
		return false;
	}
}
//Check whether the call successfully found an available channel.
//CO-CHANNELS: 0, 5, 8; 1, 4, 6; 2, 3, 7.
bool Simulate::huntingSequence(Call& call) {
	bool available = false;
	switch (call.getCell()) {
	case 0: available = availability(clusterOne, 0, call);
		break;
	case 1: available = availability(clusterTwo, 5, call);
		break;
	case 2: available = availability(clusterThree, 10, call);
		break;
	case 3: available = availability(clusterOne, 10, call);
		break;
	case 4: available = availability(clusterTwo, 5, call);
		break;
	case 5: available = availability(clusterThree, 0, call);
		break;
	case 6: available = availability(clusterOne, 5, call);
		break;
	case 7: available = availability(clusterTwo, 10, call);
		break;
	case 8: available = availability(clusterThree, 0, call);
		break;
	default: available = false;
		break;
	}

	return available;
}
//Check the availability of a given channel.
//Also pass in a "skip" to account for the hunting sequence
//to help lower the chances of co-channel interference.
bool Simulate::availability(Cluster& cluster, int skip, Call& call) {
	bool available = false;
	int clusterID = cluster.getClusterID();
	//int skipBy = 15 - skip;

	for (int i = 0; i < 15; i++) {
		//Channel is available so check if it has a good SIR.
		if (!cluster.channels.at(i)) {
			double channelSIR = SIR(i, cluster, call);
			averageSIR += channelSIR;
			cluster.channels.at(i) = true;
			if (channelSIR > 22) {
				std::cout << "Call " << call.getID() << " Successfully connected";
				std::cout << " to " << cluster.channels.at(i) + 1 << "\n";
				available = true;
				break;
			}
			else {
				cluster.channels.at(i) = false;
				std::cout << "Call " << call.getID() << " Failed to connect to ";
				std::cout << cluster.channels.at(i) + 1;
				std::cout << " Low SIR : " << channelSIR << "\n";
				available = false;
			}
		}
	}
	return available;
}

//Calculate the Signal-to-Interference ratio
//Passing the Call object will carry the cell number.
double Simulate::SIR(int channelNumber, Cluster& cluster, Call& call) {
	double sir = 0;
	//Track number of co-channel interferers.
	int interference = 0;
	int interferer;
	int interferenceFlags[3];

	//Count for interferers and store 
	for (int i = 0; i < cluster.cells.size(); i++) {
		int distance = cellDistances[call.getChannel()][cluster.cells[i]];
		if (distance > 0) {
			interference++;
		}
		interferenceFlags[i] = distance;
	}
	if (interference == 0) {
		averageSIR += 35;
		std::cout << "Interferers = 0 ";
		sir = averageSIR;

	}
	else if (interference == 1) {
		std::cout << "Interferers = 1 in cell " << call.getCell();
		interferer = cellDistances[call.getCell()][interferenceFlags[0]];
		//Calculate the Signal to Interference ratio
		sir = std::pow(1000, -4) / std::pow(interferer, -4);
	}
	//In the event there are two interferers.
	else {
		std::cout << "Interferers = 2 in cells ";
		interferer = cellDistances[call.getCell()][interferenceFlags[0]];
		int interfererTwo = cellDistances[call.getCell()][interferenceFlags[1]];
		std::cout << interferer << " and " << interfererTwo;
		double denominator = std::pow(interferer, -4) + std::pow(interfererTwo, -4);
		sir = std::pow(1000, -4) / denominator;
	}
	//Convert from SIR to dB.
	sir = 10 * std::log(sir);
	return sir;

}

double Simulate::GOS() {
	return ((accepted - blocked) / (double)totalCalls) * 100;
}

double Simulate::avgSIR() {
	return averageSIR / double(totalCalls);
}