#pragma once
//CLUSTER.H
#pragma once
#include <array>
#include <iostream>
#include "Call.h"

class Cluster {
private:
	int area = 1000;
	int id;
public:
	std::array<bool, 15> channels = { false };
	std::array<int, 3> cells;
	Cluster() {
		id++;
		//Assign cell numbers to each cluster.
		for (int i = 0; i < 3; i++) {
			cells[i] = id + i;
		}
	}
	//Allow other clusters to easily see if a channel is in use.
	bool channelInUse(int channelNumber) {
		return channels.at(channelNumber);
	}
	int getClusterID() {
		return this->id;
	}
};