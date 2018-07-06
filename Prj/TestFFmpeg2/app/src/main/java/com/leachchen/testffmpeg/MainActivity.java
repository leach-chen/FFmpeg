package com.leachchen.testffmpeg;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        this.findViewById(R.id.btn_exec).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getVideoInfo("/sdcard/vavadash/20171221_151133_003.MP4");
                //Toast.makeText(MainActivity.this,getVideoInfo("/sdcard/vavadash/20171221_151133_003.MP4"),Toast.LENGTH_LONG).show();
            }
        });
    }

    public native String getVideoInfo(String videoPath);

    static {
        System.loadLibrary("testFFmpeg");
    }
}
