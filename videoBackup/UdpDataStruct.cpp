#include "stdafx.h"
#include "UdpDataStruct.h"
#include "Tools.h"


//static int byte_to_int(char b1, char b2, char b3, char b4) {
//	return (((b1 & 0xFF) << 24) | ((b2 & 0xFF) << 16) | ((b3 & 0xFF) << 8) | (b4 & 0xFF));
//}
//
//static short byte_to_short(char b1, char b2) {
//	return (short)((b1 & 0xff) << 8 | (b2 & 0xff));
//}

UdpDataStruct::UdpDataStruct(char* udpData)
{
	tag = udpData[0];
	num = Tools::char_to_int(udpData[1], udpData[2], udpData[3], udpData[4]);
	frameTag = udpData[5];
	time = Tools::char_to_int(udpData[6], udpData[7], udpData[8], udpData[9]);
	length = Tools::char_to_short(udpData[10], udpData[11]);
	//CRC = byte_to_int(udpData[12], udpData[13], udpData[14], udpData[15]);
	videoData = new char[length];
	//	videoData = { 0 };
	ZeroMemory(videoData, length);
	memcpy(videoData, udpData + 12, length);
}

UdpDataStruct::UdpDataStruct(char * audioUdpData, int udpLen)
{
	completeAudioPacket = new char[1024];
	ZeroMemory(completeAudioPacket,1024);
	memcpy(completeAudioPacket, audioUdpData, udpLen);

	tag = completeAudioPacket[0];
	num = Tools::char_to_int(completeAudioPacket[1], completeAudioPacket[2], completeAudioPacket[3], completeAudioPacket[4]);
	time = Tools::char_to_int(completeAudioPacket[5], completeAudioPacket[6], completeAudioPacket[7], completeAudioPacket[8]);
	length = Tools::char_to_short(completeAudioPacket[9], completeAudioPacket[10]);
	
	audioData = new char[length];
	ZeroMemory(audioData, length);
	memcpy(audioData, audioUdpData + 11, length);
	audioLenShift = length + 11;



////利用ffmpeg连续解码音频性能
//	tag = audioUdpData[0];
//	num = Tools::char_to_int(audioUdpData[1],audioUdpData[2],audioUdpData[3],audioUdpData[4]);
//	time = Tools::char_to_int(audioUdpData[5],audioUdpData[6],audioUdpData[7],audioUdpData[8]);
//	length = Tools::char_to_short(audioUdpData[9], audioUdpData[10]);
//
//
//    	audioDataLen = udpLen - (5+6*5);
//	    audioData = new char[audioDataLen];
//		
//		audioLenShift = length + 11;
//		memcpy(audioData, audioUdpData +11, length);
//		for (int i = 0; i < 4; i++)
//		{
//			length = Tools::char_to_short(audioUdpData[audioLenShift + 4], audioUdpData[audioLenShift + 5]);
//			memcpy(audioData, audioUdpData + audioLenShift + 6, length);
//			audioLenShift += (length + 6);
//        }
}


UdpDataStruct::~UdpDataStruct()
{
	//	delete[] videoData;
	TRACE(_T("udpStruct destroy\n"));
}

int UdpDataStruct::getTag()
{
	return tag;
}

int UdpDataStruct::getFrameTag()
{
	return frameTag;
}

int UdpDataStruct::getNum()
{
	return num;
}

int UdpDataStruct::getTime()
{
	return time;
}

int UdpDataStruct::getCRC()
{
	return CRC;
}

int UdpDataStruct::getLength()
{
	return length;
}

int UdpDataStruct::getAudioDataLen()
{
	return audioDataLen;
}

char * UdpDataStruct::getVideoData()
{
	return videoData;
}

char * UdpDataStruct::getAudioData()
{
	return audioData;
}

void UdpDataStruct::nextAudioShift()
{
	time = Tools::char_to_int(completeAudioPacket[audioLenShift], completeAudioPacket[audioLenShift+1], completeAudioPacket[audioLenShift+2], completeAudioPacket[audioLenShift+3]);
	length = Tools::char_to_short(completeAudioPacket[audioLenShift+4], completeAudioPacket[audioLenShift+5]);

	audioData = new char[length];
	ZeroMemory(audioData, length);
	memcpy(audioData, completeAudioPacket+ audioLenShift + 6, length);
	audioLenShift += (6 + length);
}

