package com.entboost.im.push;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import android.annotation.SuppressLint;
import android.content.Context;
import android.text.TextUtils;

//import com.entboost.Log4jLog;
import com.xiaomi.mipush.sdk.ErrorCode;
import com.xiaomi.mipush.sdk.MiPushClient;
import com.xiaomi.mipush.sdk.MiPushCommandMessage;
import com.xiaomi.mipush.sdk.MiPushMessage;
import com.xiaomi.mipush.sdk.PushMessageReceiver;

public class XiaoMiPushReceiver extends PushMessageReceiver {

//	private static String LONG_TAG = XiaoMiPushReceiver.class.getName();
	
    private String mRegId;
    private String mTopic;
    private String mAlias;
    private String mAccount;
    private String mStartTime;
    private String mEndTime;

    @Override
    public void onReceivePassThroughMessage(Context context, MiPushMessage message) {
    	//注意：本回调在非主UI线程
//        Log4jLog.d(LONG_TAG, "onReceivePassThroughMessage is called. " + message.toString());
        
        if (!TextUtils.isEmpty(message.getTopic())) {
            mTopic = message.getTopic();
        } else if (!TextUtils.isEmpty(message.getAlias())) {
            mAlias = message.getAlias();
        }
    }

    @Override
    public void onNotificationMessageClicked(Context context, MiPushMessage message) {
    	//注意：本回调在非主UI线程
//        Log4jLog.d(LONG_TAG, "onNotificationMessageClicked is called. " + message.toString());

        if (!TextUtils.isEmpty(message.getTopic())) {
            mTopic = message.getTopic();
        } else if (!TextUtils.isEmpty(message.getAlias())) {
            mAlias = message.getAlias();
        }
    }
    
    @Override
    public void onNotificationMessageArrived(Context context, MiPushMessage message) {
    	//注意：本回调在非主UI线程
//        Log4jLog.d(LONG_TAG, "onNotificationMessageArrived is called. " + message.toString());

        if (!TextUtils.isEmpty(message.getTopic())) {
            mTopic = message.getTopic();
        } else if (!TextUtils.isEmpty(message.getAlias())) {
            mAlias = message.getAlias();
        }
    }
    
    @Override
    public void onCommandResult(Context context, MiPushCommandMessage message) {
    	//注意：本回调在非主UI线程
//        Log4jLog.d(LONG_TAG, "onCommandResult is called. " + message.toString());
        
        String command = message.getCommand();
        List<String> arguments = message.getCommandArguments();
        String cmdArg1 = ((arguments != null && arguments.size() > 0) ? arguments.get(0) : null);
        String cmdArg2 = ((arguments != null && arguments.size() > 1) ? arguments.get(1) : null);
        String log;
        if (MiPushClient.COMMAND_REGISTER.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mRegId = cmdArg1;
                log = "register_success";
            } else {
                log = "register_fail";
            }
        } else if (MiPushClient.COMMAND_SET_ALIAS.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mAlias = cmdArg1;
                log = "set_alias_success: " + mAlias;
            } else {
                log = "set_alias_fail: " + message.getReason();
            }
        } else if (MiPushClient.COMMAND_UNSET_ALIAS.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mAlias = cmdArg1;
                log = "unset_alias_success: " + mAlias;
            } else {
                log = "unset_alias_fail: " + message.getReason();
            }
        } else if (MiPushClient.COMMAND_SET_ACCOUNT.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mAccount = cmdArg1;
                log = "set_account_success: " + mAccount;
            } else {
                log = "set_account_fail: " + message.getReason();
            }
        } else if (MiPushClient.COMMAND_UNSET_ACCOUNT.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mAccount = cmdArg1;
                log = "unset_account_success" + mAccount;
            } else {
                log = "unset_account_fail: " +  message.getReason();
            }
        } else if (MiPushClient.COMMAND_SUBSCRIBE_TOPIC.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mTopic = cmdArg1;
                log = "subscribe_topic_success: " + mTopic;
            } else {
                log = "subscribe_topic_fail: " + message.getReason();
            }
        } else if (MiPushClient.COMMAND_UNSUBSCRIBE_TOPIC.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mTopic = cmdArg1;
                log = "unsubscribe_topic_success: " + mTopic;
            } else {
                log = "unsubscribe_topic_fail: " + message.getReason();
            }
        } else if (MiPushClient.COMMAND_SET_ACCEPT_TIME.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mStartTime = cmdArg1;
                mEndTime = cmdArg2;
                log = "set_accept_time_success: " + mStartTime + "  " + mEndTime;
            } else {
                log = "set_accept_time_fail: " + message.getReason();
            }
        } else {
            log = message.getReason();
        }
        
//        Log4jLog.d(LONG_TAG, log);
    }

    @Override
    public void onReceiveRegisterResult(Context context, MiPushCommandMessage message) {
    	//注意：本回调在非主UI线程
//        Log4jLog.d(LONG_TAG, "onReceiveRegisterResult is called. " + message.toString());

        /// for test
//        EntboostPushClient.pushToken = "ret: " + message.getReason();
        String command = message.getCommand();
        List<String> arguments = message.getCommandArguments();
        String cmdArg1 = ((arguments != null && arguments.size() > 0) ? arguments.get(0) : null);
        String log;
        if (MiPushClient.COMMAND_REGISTER.equals(command)) {
            if (message.getResultCode() == ErrorCode.SUCCESS) {
                mRegId = cmdArg1;
                log = "register_success, mRegId = " + mRegId;

                /// ***
                EntboostPushClient.pushToken = mRegId;
//                ThirdPartyPushHelper.pushToken = mRegId;
//                ThirdPartyPushHelper.setPushToken(true);
            } else {
                log = "register_fail: " + message.getReason();
            }
        } else {
            log = message.getReason();
        }
        
//        Log4jLog.d(LONG_TAG, log);
    }

    @SuppressLint("SimpleDateFormat")
    private static String getSimpleDate() {
        return new SimpleDateFormat("MM-dd hh:mm:ss").format(new Date());
    }

}
