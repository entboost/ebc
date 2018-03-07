package org.qtproject.entboost;

import org.qtproject.entboost.R;
import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtService;
import org.qtproject.entboost.MyCustomAppService;
import android.content.Intent;
import android.util.Log;
import android.os.Bundle;

public class MyCustomAppActivity extends QtActivity {
    @Override
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        Log.i("Activity", "Starting service!");
        Intent serviceIntent = new Intent(this, org.qtproject.entboost.MyCustomAppService.class);
        startService(serviceIntent);
    }
    @Override
    protected void onResume() {
        super.onResume();
    }
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }
    @Override
    public void onPause() {
        super.onPause();
    }
    @Override
    public void onDestroy() {
        super.onDestroy();
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
    }
}
