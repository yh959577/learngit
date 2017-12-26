#pragma once
class UdpDataStruct
{
public:
	UdpDataStruct(char* udpData);
	UdpDataStruct(char* audioData,int length);
	~UdpDataStruct();
	int getTag();
	int getFrameTag();
	int getNum();
	int getTime();
	int getCRC();
	int getLength();
	int getAudioDataLen();
	char* getVideoData();
	char* getAudioData();
	void nextAudioShift();

private:
	int tag;
	int frameTag;
	short length;
	int num;	
	int time;
	int CRC;
	int audioLenShift;
	int audioDataLen;
	char* videoData;
	char* audioData;
	char* completeAudioPacket;


};

