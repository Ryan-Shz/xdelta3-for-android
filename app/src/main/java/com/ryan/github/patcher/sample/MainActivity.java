package com.ryan.github.patcher.sample;

import android.os.Environment;
import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.ryan.github.patcher.Patcher;

import java.io.File;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    public static final String TAG = "Patcher";
    private ExecutorService mExecutorService;
    private EditText mPrefixEd;
    private Button mApplyPatchBtn;

    private static final String testDir = Environment.getExternalStorageDirectory() + File.separator + "patch_test" + File.separator;
    private static final String PATCH_SUFFIX = "_patch.patch";
    private static final String OLD_APK_SUFFIX = "_old.apk";
    private static final String NEW_APK_SUFFIX = "_new.apk";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mExecutorService = Executors.newCachedThreadPool();
        ensureTestDirExists();
        mPrefixEd = findViewById(R.id.prefix_ed);
        mApplyPatchBtn = findViewById(R.id.apply_btn);
        mApplyPatchBtn.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        String prefix = mPrefixEd.getText().toString().trim();
        if (TextUtils.isEmpty(prefix)) {
            showToast("please enter apk name.");
            return;
        }
        ensureTestDirExists();
        String apkPath = testDir + prefix;
        if (v == mApplyPatchBtn) {
            applyPatch(apkPath);
        }
    }

    private void applyPatch(final String apkName) {
        runOnWorkerThread(new Runnable() {
            @Override
            public void run() {
                final String inFilePath = apkName + PATCH_SUFFIX;
                final String srcFilePath = apkName + OLD_APK_SUFFIX;
                final String outFilePath = apkName + NEW_APK_SUFFIX;
                long start = SystemClock.currentThreadTimeMillis();
                int res = Patcher.apply(inFilePath, srcFilePath, outFilePath);
                Log.v(TAG, "apply patch time: " + (SystemClock.currentThreadTimeMillis() - start));
                if (res == 0) {
                    showToast("apply patch success!");
                } else {
                    showToast("app patch error! res code: " + res);
                }
            }
        });
    }

    private void runOnWorkerThread(Runnable r) {
        mExecutorService.submit(r);
    }

    private void showToast(final String message) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.this, message, Toast.LENGTH_SHORT).show();
            }
        });
    }

    private void ensureTestDirExists() {
        File testDirFile = new File(testDir);
        if (!testDirFile.exists()) {
            testDirFile.mkdir();
        }
    }
}
