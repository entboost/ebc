package org.qtproject.entboost;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.SharedPreferences;

public class BootBroadcastReceiver extends BroadcastReceiver
{
    @Override
    public void onReceive(Context context, Intent intent)
    {
        Intent boot = new Intent(context, org.qtproject.qt5.android.bindings.QtActivity.class);
        boot.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(boot);
    }
}
