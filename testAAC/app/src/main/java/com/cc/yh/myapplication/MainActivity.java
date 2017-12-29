package com.cc.yh.myapplication;

import android.app.Activity;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.widget.Button;

import java.io.IOException;

public class MainActivity extends Activity implements AudioCodec {
    Button btnRecorder;
    Button btnPlay;
    Button btnRecorderAAC;
    AudioRecord mAudioRecord;
    int recBufSize;
    byte[] pcmData;
    boolean isRecording;
    Thread recodingThread;
    AudioEncoder mAudioEncoder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        btnRecorder = findViewById(R.id.record_AAC);
        btnPlay = findViewById(R.id.play_AAC);
        btnRecorderAAC=findViewById(R.id.button);
        createAudio();
       // RecordAACFile();
        mAudioEncoder=new AudioEncoder(recBufSize);
        btnRecorder.setOnClickListener(view -> {
            if (btnRecorder.getText() == getString(R.string.start_recorder_AAC)) {
                btnRecorder.setText(R.string.stop_recorder_AAC);
                isRecording = true;
                mAudioRecord.startRecording();
                recodingThread.start();
               // mediaRecorder.start();
            } else {
                btnRecorder.setText(R.string.start_recorder_AAC);
                isRecording = false;
              mAudioRecord.stop();
                recodingThread.interrupt();
              //  mediaRecorder.stop();
            }
        });


        btnPlay.setOnClickListener(view -> {
            if (btnPlay.getText() == getString(R.string.play_AAC)) {
                btnPlay.setText(R.string.stop_play);
            } else {
                btnPlay.setText(R.string.play_AAC);
            }
        });
        recodingThread = new Thread(this::startEncode);

        btnRecorderAAC.setOnClickListener(v->{


        });

    }
    MediaRecorder mediaRecorder=new MediaRecorder();
    private void RecordAACFile(){
        mediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
          mediaRecorder.setAudioChannels(2);

          mediaRecorder.setAudioEncodingBitRate(KEY_BIT_RATE);
          mediaRecorder.setAudioSamplingRate(KEY_SAMPLE_RATE);
        //  mediaRecorder.setOutputFile(new File(Environment.getExternalStorageDirectory().getPath() + "/testAACFile.aac"));
           mediaRecorder.setOutputFile(Environment.getExternalStorageDirectory().getPath() + "/testAACFile.aac");
           mediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.AAC_ADTS);
        mediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
        try {
            mediaRecorder.prepare();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    private void startEncode() {
      int read=0;
          while (isRecording){
            read=mAudioRecord.read(pcmData,0,recBufSize);
            if (read!=AudioRecord.ERROR_INVALID_OPERATION){
                   mAudioEncoder.offerEncoder(pcmData);
            }

          }

    }

    private void createAudio() {

        recBufSize = AudioRecord.getMinBufferSize(KEY_SAMPLE_RATE, CHANNEL_MODE , AUDIO_FORMAT);
        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, KEY_SAMPLE_RATE, CHANNEL_MODE,
                AUDIO_FORMAT, recBufSize);
        pcmData = new byte[recBufSize];
    }


}
