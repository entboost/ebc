package org.qtproject.entboost;

import android.app.Service;
import android.os.IBinder;
import android.content.Intent;
import android.os.Bundle;
import org.qtproject.qt5.android.bindings.QtService;
import android.util.Log;

public class MyCustomAppService extends QtService {

   /** Called when the service is being created. */
   @Override
   public void onCreate() {
      super.onCreate();
      Log.i("Service", "Service created!");
   }

   /** The service is starting, due to a call to startService() */
   @Override
   public int onStartCommand(Intent intent, int flags, int startId) {
      int ret = super.onStartCommand(intent, flags, startId);
      Log.i("Service", "Service created!");
      return ret;
      //return mStartMode;
   }

   /** A client is binding to the service with bindService() */
   @Override
   public IBinder onBind(Intent intent) {
      return super.onBind(intent);
      //return mBinder;
   }

   /** Called when all clients have unbound with unbindService() */
   @Override
   public boolean onUnbind(Intent intent) {
      return super.onUnbind(intent);
   }

   /** Called when a client is binding to the service with bindService()*/
   @Override
   public void onRebind(Intent intent) {
         super.onRebind(intent);
   }

   /** Called when The service is no longer used and is being destroyed */
   @Override
   public void onDestroy() {
      super.onDestroy();
   }
}
