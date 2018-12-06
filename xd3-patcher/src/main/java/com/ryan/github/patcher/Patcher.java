package com.ryan.github.patcher;

/**
 * Created by Ryan
 */
public final class Patcher {

    private static boolean sLoadNativeSuccess;
    private static final String LIB_NAME = "xd3-patcher";

    static {
        try {
            System.loadLibrary(LIB_NAME);
            sLoadNativeSuccess = true;
        } catch (Exception e) {
            sLoadNativeSuccess = false;
        }
    }

    /**
     * 合成Patch
     *
     * @param inFilePath  patch包路径
     * @param srcFilePath 旧版Apk包路径
     * @param outFilePath 合成的新包路径
     * @return 处理结果状态码，0成功/其他失败
     */
    private static native int applyPatch(String inFilePath, String srcFilePath, String outFilePath);

    public static int apply(String inFilePath, String srcFilePath, String outFilePath) {
        if (sLoadNativeSuccess) {
            return applyPatch(inFilePath, srcFilePath, outFilePath);
        }
        return -1;
    }

}
