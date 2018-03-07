package com.entboost.im.push;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;

//import com.entboost.Log4jLog;
import com.huawei.hms.api.ConnectionResult;
import com.huawei.hms.api.HuaweiApiClient;
import com.huawei.hms.api.HuaweiApiClient.ConnectionCallbacks;
import com.huawei.hms.api.HuaweiApiClient.OnConnectionFailedListener;
import com.huawei.hms.support.api.client.PendingResult;
import com.huawei.hms.support.api.push.HuaweiPush;
import com.huawei.hms.support.api.push.TokenResult;

/**
 * 华为Push接口管理类
 *
 */
public class HuaweiPushApiAccessor implements ConnectionCallbacks, OnConnectionFailedListener {

	private static String LONG_TAG = HuaweiPushApiAccessor.class.getName();
    
	/*
    // 接收Push消息
    public static final int RECEIVE_PUSH_MSG = 0x100;

    // 接收Push Token消息
    public static final int RECEIVE_TOKEN_MSG = 0x101;

    // 接收Push 自定义通知消息内容
    public static final int RECEIVE_NOTIFY_CLICK_MSG = 0x102;

    public static final int RECEIVE_TAG_MSG = 0x103;

    public static final int RECEIVE_STATUS_MSG = 0x104;

    public static final int OTHER_MSG = 0x105;

    public static final String NORMAL_MSG_ENABLE = "normal_msg_enable";

    public static final String NOTIFY_MSG_ENABLE = "notify_msg_enable";
    */
    
	//上下文
	private Context context;
	
	//事件监听器
    private HuaweiPushApiAccessorListener listener;
    //连接华为Push服务器的访问对象
    private HuaweiApiClient huaweiApiClient;
    //全局单实例
    private static HuaweiPushApiAccessor instance;
    
    
	public void setListener(HuaweiPushApiAccessorListener listener) {
		this.listener = listener;
	}

	public static HuaweiPushApiAccessor getInstance(Context context) {
    	synchronized (HuaweiPushApiAccessor.class) {
			if (instance==null) {
				instance = new HuaweiPushApiAccessor();
				instance.create(context);
				//instance.connect();
			}
		}
    	return instance;
    }

	/**
     * 创建连接华为服务的访问对象
     * @param context app上下文
     */
    public void create(Context context) {
    	this.context = context;
    	//HuaweiIdSignInOptions options = new HuaweiIdSignInOptions.Builder(HuaweiIdSignInOptions.DEFAULT_SIGN_IN).build();
    	
        huaweiApiClient = new HuaweiApiClient.Builder(context)
        		//.addApi(HuaweiId.SIGN_IN_API, options)
        		.addApi(HuaweiPush.PUSH_API)
        		//.addScope(HuaweiId.HUAEWEIID_BASE_SCOPE)
        		.addConnectionCallbacks(this)
                .addOnConnectionFailedListener(this)
                .build();
    }
    
    /**
     * 连接到华为服务
     */
    public void connect() {
    	if (huaweiApiClient!=null)
    		huaweiApiClient.connect();
    }
    
    /**
     * 断开连接华为服务
     */
    public void disconnect() {
    	if (huaweiApiClient!=null)
    		huaweiApiClient.disconnect();
    }
    
    @Override
    public void onConnected() {
//    	Log4jLog.i(LONG_TAG, "onConnected, IsConnected: " + huaweiApiClient.isConnected());
    	if (listener!=null)
    		listener.onConnected();
    }
    
    @Override
    public void onConnectionFailed(ConnectionResult result) {
    	final int errorCode = result.getErrorCode();
        /// 6003=签名文件错误
        /// for test
//        EntboostPushClient.pushToken = String.valueOf(errorCode);

//    	Log4jLog.e(LONG_TAG, "onConnectionFailed, ErrorCode: " + errorCode);
    	
    	//检查是否显示安装升级华为服务组件的界面
		SharedPreferences preferences = context.getSharedPreferences("first", Context.MODE_PRIVATE);
		boolean enableShow = preferences.getBoolean(HuaweiResolveErrorActivity.ENABLE_SHOW_HUAWEI_RESOLVE_ERROR_VIEW, true);
		
		if (enableShow) {
	    	//延迟5秒后执行
	    	new Handler().postDelayed(new Runnable() {
				@Override
				public void run() {
			    	Intent intent = new Intent(context, HuaweiResolveErrorActivity.class);
					intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_CLEAR_TOP);
					intent.putExtra(HuaweiResolveErrorActivity.INTENT_ERROR_CODE, errorCode);
					
			    	context.startActivity(intent);
				}
			}, 10000);
		}
    }
    
    @Override
    public void onConnectionSuspended(int cause) {
//    	Log4jLog.i(LONG_TAG, "onConnectionSuspended, cause: " + cause + ", IsConnected: " + huaweiApiClient.isConnected());
    }
    
    /*
    @TestMethod(name = "getToken")
    public void t00_getToken() {
        getToken();
    }

    @TestMethod(name = "deleteToken")
    public void t01_delToken() {
        Intent intent = new Intent(HuaweiPushApi.this, HuaweiPushDelTokenApiExample.class);
        startActivity(intent);
    }

    @TestMethod(name = "getPushState")
    public void t03_getState() {
        getState();
    }

    @TestMethod(name = "enablePassByMsg")
    public void t04_enablePassByMsg() {
        setPassByMsg(true);
        Log.i(TAG, "enable receive pass-by notification.");
    }

    @TestMethod(name = "disablePassByMsg")
    public void t05_disablePassByMsg() {
        setPassByMsg(false);
        Log.i(TAG, "disable receive pass-by notification.");
    }
	*/
    
    /**
     * 设置透传消息开关
     * @param flag 是否透传消息
     */
    public void setPassByMsg(boolean flag) {
        HuaweiPush.HuaweiPushApi.enableReceiveNormalMsg(huaweiApiClient, flag);
    }
    
    /**
     * 获取Push Token(结果通过广播返回)
     */
    public void requestTokenFormHuaweiServer() {
        if (!isConnected()) {
//            Log4jLog.e(LONG_TAG, "request token failed, HMS is disconnect.");
            return;
        }
        
        // 同步调用方式
        new Thread() {
        	@Override public void run() {
        		PendingResult<TokenResult> tokenResult = HuaweiPush.HuaweiPushApi.getToken(huaweiApiClient); 
        		// 结果通过广播返回，不通过pendingResult返回，预留接口 
        		tokenResult.await();
        	}
        }.start();
        
        // 异步调用方式
        /*
        PendingResult<TokenResult> tokenResult = HuaweiPush.HuaweiPushApi.getToken(huaweiApiClient);
        tokenResult.setResultCallback(new ResultCallback<TokenResult>() {
            @Override
            public void onResult(TokenResult result) {
            	Log4jLog.d(LONG_TAG, "getToken status:" + result.getStatus().getStatusCode());
            	if (result.getTokenRes()!=null) {
            		Log4jLog.d(LONG_TAG, "token:" + result.getTokenRes().getToken());
            	}
            }
        });*/
    }
    
    /**
     * 读取华为Push状态(内部新开子线程执行，结果通过广播返回)
     */
    public void getPushState() {
        if (!isConnected()) {
//        	Log4jLog.e(LONG_TAG, "get push state failed, HMS is disconnect.");
            return;
        }
        
        new Thread() {
            @Override
            public void run() {
                // 状态结果通过广播返回
                HuaweiPush.HuaweiPushApi.getPushState(huaweiApiClient);
            }
        }.start();
    }
    
    /**
     * 判断是否已连接到华为服务
     * @return
     */
    public boolean isConnected() {
        if (huaweiApiClient != null && huaweiApiClient.isConnected()) {
            return true;
        } else {
            return false;
        }
    }
    
    /**
     * 判断是否正在连接中
     * @return
     */
    public boolean isConnecting() {
    	if (huaweiApiClient != null && huaweiApiClient.isConnecting()) {
            return true;
        } else {
            return false;
        }
    }
    
    /**
     * 定义监听器接口
     *
     */
    public interface HuaweiPushApiAccessorListener {
    	public void onConnected();
    }
}
