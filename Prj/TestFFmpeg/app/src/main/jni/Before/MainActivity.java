package Before;/*
package Before;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import com.leachchen.testffmpeg.R;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback{

    public TextView test_tv;
    public SurfaceView surface_sv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        test_tv = (TextView)this.findViewById(R.id.test_tv);
        surface_sv = (SurfaceView)this.findViewById(R.id.surface_sv);
        surface_sv.getHolder().addCallback(this);

    }

    public void updateInfo(final String info)
    {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                test_tv.setText(info);
            }
        });
    }

    public native String test(String str);
    public native String getVideoInfo(String filePath);
    public native int playByNativeWindow(String filePath, Surface surface);


    public void onPlay(View v)
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                //playByNativeWindow("/sdcard/vava/a.MP4",surface_sv.getHolder().getSurface());
                getVideoInfo("/sdcard/vavadash/20171221_151133_003.MP4");
            }
        }).start();
    }


    static {
        System.loadLibrary("VideoPlayer");
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }
}
*/
