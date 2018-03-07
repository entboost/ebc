package com.entboost.im.push;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;

//import com.entboost.Log4jLog;
//import com.entboost.im.R;
import org.qtproject.entboost.R;
import com.huawei.hms.api.ConnectionResult;
import com.huawei.hms.api.HuaweiApiAvailability;
import com.huawei.hms.api.HuaweiApiAvailability.OnUpdateListener;

public class HuaweiResolveErrorActivity extends Activity implements OnUpdateListener {
	
	private static String LONG_TAG = HuaweiResolveErrorActivity.class.getName();
	
	public static String INTENT_ERROR_CODE = "errorCode";
	
    private static final int REQUEST_RESOLVE_ERROR = 1001;
    
    @Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
//                setContentView(R.layout.hms_download_dialog);
                setContentView(R.layout.activity_huawei_resolve_error);
		
		executeResolveError();
	}
    
	@Override
	protected void onNewIntent(Intent intent) {
		setIntent(intent);
		
		executeResolveError();
		super.onNewIntent(intent);
	}
    
	/**
	 * 是否显示安装升级华为服务组件的界面
	 */
	public static String ENABLE_SHOW_HUAWEI_RESOLVE_ERROR_VIEW = "enable_show_huawei_resolve_error_view";
	
	private void executeResolveError() {
		Intent intent = getIntent();
		
                //获取CheckBox实例
                CheckBox cb = (CheckBox)this.findViewById(R.id.checkBox1);
                //绑定监听器
                cb.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton arg0, boolean checkedState) {
                        SharedPreferences preferences = getSharedPreferences("first", Context.MODE_PRIVATE);
                        SharedPreferences.Editor editor = preferences.edit();
                        editor.putBoolean(ENABLE_SHOW_HUAWEI_RESOLVE_ERROR_VIEW, !checkedState);
                        editor.commit();
//	    		Log4jLog.d(LONG_TAG, "set enable_show_huawei_resolve_error_view:" + !checkedState);
            }
                });
		
		int errorCode = intent.getIntExtra(INTENT_ERROR_CODE, -1);
        HuaweiApiAvailability availability = HuaweiApiAvailability.getInstance();
        
        if (availability.isUserResolvableError(errorCode)) {
            availability.resolveError(this, errorCode, REQUEST_RESOLVE_ERROR, this);
        }
	}
	
	@Override
    public void onUpdateFailed(ConnectionResult result) {
    	int errorCode = result.getErrorCode();
//    	Log4jLog.e(LONG_TAG, "onUpdateFailed, ErrorCode: " + errorCode);
    	

        HuaweiApiAvailability availability = HuaweiApiAvailability.getInstance();
        availability.resolveError(this, errorCode, REQUEST_RESOLVE_ERROR, this);
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_RESOLVE_ERROR) {
//        	Log4jLog.i(LONG_TAG, "onActivityResult, ResultCode: " + resultCode + ", Intent: " + data);
            
        	try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
//				Log4jLog.e(LONG_TAG, e);
			}
        	
            int result = HuaweiApiAvailability.getInstance().isHuaweiMobileServicesAvailable(this);
//            Log4jLog.i(LONG_TAG, "onActivityResult, isHuaweiMobileServicesAvailable result: " + result);
            
            if (result == ConnectionResult.SUCCESS || result == ConnectionResult.SERVICE_UPDATING) {
            	HuaweiPushApiAccessor hwpAccessor = HuaweiPushApiAccessor.getInstance(this);
                if (!hwpAccessor.isConnecting() && !hwpAccessor.isConnected()) {
                	hwpAccessor.connect();
                }
            } else {
//                Log4jLog.e(LONG_TAG, "onActivityResult, HuaweiMobileServices is not available:" + result);
            }
        }
        
        finish();
    }
    
}
