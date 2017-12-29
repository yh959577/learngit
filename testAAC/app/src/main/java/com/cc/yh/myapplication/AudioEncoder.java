package com.cc.yh.myapplication;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Environment;
import android.util.Log;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;

/**
 * Created by Administrator on 2017/12/6.
 */

public class AudioEncoder implements AudioCodec {
    private MediaCodec mediaCodec;
    private BufferedOutputStream outputStream;
    // private String mediaType = "OMX.google.aac.encoder";
    private  byte[] information;
    private DatagramSocket mSocket;
    private DatagramPacket mPacket;
     private  String HOST="192.168.2.103";
   private final String TAG="AudioEncoder";


// "OMX.qcom.audio.decoder.aac";
// "audio/mp4a-latm";


    public AudioEncoder(int recBufSize) {
        try {
            mSocket=new DatagramSocket();
        } catch (SocketException e) {
            e.printStackTrace();
        }

        File f = new File(Environment.getExternalStorageDirectory().getPath() + "/testAAC.aac");
        //   File f = new File(Environment.getExternalStorageDirectory(),
        //   "AACTest/audio_encoded.aac");
        //   touch(f);
        try {
            outputStream = new BufferedOutputStream(new FileOutputStream(f));
            Log.e("AudioEncoder", "outputStream initialized");
        } catch (Exception e) {
            e.printStackTrace();
        }
        try {
            mediaCodec = MediaCodec.createEncoderByType(MIME_TYPE);
        } catch (IOException e) {
            e.printStackTrace();
        }
//        MediaFormat mediaFormat = MediaFormat.createAudioFormat(
//                MediaFormat.MIMETYPE_AUDIO_AMR_WB, kSampleRates[3], 2);
////        mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE,
////                MediaCodecInfo.CodecProfileLevel.AACObjectLC);
//        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, kBitRates[1]);
//        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 8192);// It will
//// increase
//// capacity
//// of
// inputBuffers
//        mediaCodec.configure(mediaFormat, null, null,
//                MediaCodec.CONFIGURE_FLAG_ENCODE);
//        mediaCodec.start();
        MediaFormat mediaFormat=new MediaFormat();
        mediaFormat.setString(MediaFormat.KEY_MIME, MIME_TYPE);
        mediaFormat.setInteger(MediaFormat.KEY_CHANNEL_COUNT, KEY_CHANNEL_COUNT);
        mediaFormat.setInteger(MediaFormat.KEY_SAMPLE_RATE, KEY_SAMPLE_RATE);
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, KEY_BIT_RATE);// AAC-HE // 64kbps
        mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, KEY_AAC_PROFILE);//AACObjectLC
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE ,recBufSize*2);
        mediaCodec.configure(mediaFormat, null, null,MediaCodec.CONFIGURE_FLAG_ENCODE);
        mediaCodec.start();


    }




    public void close() {
        try {
            mediaCodec.stop();
            mediaCodec.release();
            outputStream.flush();
            outputStream.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    // called AudioRecord's read
    public  void offerEncoder(byte[] input) {
        Log.e("AudioEncoder", input.length + " is coming");
        ByteBuffer[] inputBuffers;
        ByteBuffer[] outputBuffers;
        inputBuffers = mediaCodec.getInputBuffers();
        outputBuffers = mediaCodec.getOutputBuffers();
        int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);


        if (inputBufferIndex >= 0) {
            ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
            inputBuffer.clear();
            inputBuffer.put(input);
            mediaCodec.queueInputBuffer(inputBufferIndex, 0, input.length, System.nanoTime()/1000, 0);
        }


        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);


 //trying to add a ADTS
        while (outputBufferIndex >= 0) {
            int outBitsSize = bufferInfo.size;
            int outPacketSize = outBitsSize + 7; // 7 is ADTS size
            ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];


            outputBuffer.position(bufferInfo.offset);
            outputBuffer.limit(bufferInfo.offset + outBitsSize);


            byte[] outData = new byte[outPacketSize];
            addADTStoPacket(outData, outPacketSize);


            outputBuffer.get(outData, 7, outBitsSize);
            outputBuffer.position(bufferInfo.offset);


// byte[] outData = new byte[bufferInfo.size];
            try {
                outputStream.write(outData, 0, outData.length);
                Log.i(TAG, "offerEncoderLength=====: "+outData.length);
                sendUdp(outData);
            } catch (IOException e) {
// TODO Auto-generated catch block
                e.printStackTrace();
            }
            Log.e("AudioEncoder", outData.length + " bytes written");


            mediaCodec.releaseOutputBuffer(outputBufferIndex, false);
            outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
        }
    }

    private void sendUdp(byte[] udpData){
        try {
            mPacket=new DatagramPacket(udpData,udpData.length, InetAddress.getByName(HOST),8080);
            mSocket.send(mPacket);
        } catch (IOException e) {
            e.printStackTrace();
        }
      //  Log.i(TAG, "sendUdpDataLen: "+udpData.length);
     //   Log.i(TAG, "sendUdpDataContent: "+ Arrays.toString(udpData));


    }


// Without ADTS header
//
//        while (outputBufferIndex >= 0) {
//            ByteBuffer outputBuffer =
//                    outputBuffers[outputBufferIndex];
//            byte[] outData = new
//                    byte[bufferInfo.size];
//
//            outputBuffer.get(outData);
//
//                try {
//                    outputStream.write(outData, 0, outData.length);
//                    Log.i("xieRu", "offerEncoderWriteIn: "+ Arrays.toString(outData));
//                } catch (IOException e) { // TODO Auto-generated
//                    e.printStackTrace();
//                }
//                Log.e("AudioEncoder", outData.length + " bytes written");
//
//            mediaCodec.releaseOutputBuffer(outputBufferIndex, false);
//            outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
//        }
//    }


    /**
     * Add ADTS header at the beginning of each and every AAC packet. This is
     * needed as MediaCodec encoder generates a packet of raw AAC data.
     * <p>
     * Note the packetLen must count in the ADTS header itself.
     **/
    private void addADTStoPacket(byte[] packet, int packetLen) {
        int profile = 2; // AAC LC
// 39=MediaCodecInfo.CodecProfileLevel.AACObjectELD;
        int freqIdx = 4; // 44.1KHz
        int chanCfg = 2; // CPE
       // int chanCfg = 3;

// fill in ADTS data
        packet[0] = (byte) 0xFF;
        packet[1] = (byte) 0xF9;
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }


    private void touch(File f) {
        try {
            if (!f.exists())
                f.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
