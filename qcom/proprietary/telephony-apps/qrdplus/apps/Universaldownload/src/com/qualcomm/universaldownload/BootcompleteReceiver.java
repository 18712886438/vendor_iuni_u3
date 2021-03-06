/*
 *
 * Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/
package com.qualcomm.universaldownload;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class BootcompleteReceiver extends BroadcastReceiver {
    public void onReceive(Context context, Intent intent) {
        Intent i = new Intent(intent);
        i.setClass(context, DownloadService.class);
        context.startService(i);
    }
}
