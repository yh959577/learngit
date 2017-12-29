package com.cc.yh.myapplication;

import android.media.AudioFormat;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;

/**
 * Created by Administrator on 2017/12/6.
 */

public interface AudioCodec {
    String MIME_TYPE= MediaFormat.MIMETYPE_AUDIO_AAC;
    int KEY_CHANNEL_COUNT=2;
    int KEY_SAMPLE_RATE=44100;
    int KEY_BIT_RATE=32000;
    int KEY_AAC_PROFILE= MediaCodecInfo.CodecProfileLevel.AACObjectLC;
    int WAIT_TIME=10000;
    int AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;
    int CHANNEL_MODE = AudioFormat.CHANNEL_IN_STEREO;
    int BUFFFER_SIZE=2048;
}
