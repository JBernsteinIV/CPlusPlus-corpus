#pragma once
//CALL.H
#pragma once
class Call {
private:
	int id, time, cell, duration, channel;
	bool connected;
public:
	Call(int id, int time, int cell, int duration) {
		this->id = id;
		this->time = time;
		this->cell = cell;
		this->duration = duration;
		this->connected = false;
	}
	bool connectionStatus() {
		return this->connected;
	}
	int getID() {
		return this->id;
	}
	int getTime() {
		return this->time;
	}
	int getCell() {
		return this->cell;
	}
	int getDuration() {
		return this->duration;
	}
	int getChannel() {
		return this->channel;
	}
	void setChannel(int channelNumber) {
		this->channel = channelNumber;
	}
	void connect() {
		this->connected = true;
	}
	void disconnect() {
		this->connected = false;
	}
};
