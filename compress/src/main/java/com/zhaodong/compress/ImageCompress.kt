package com.zhaodong.compress

import android.graphics.Bitmap

class ImageCompress {
    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    /**
     * 使用native方法进行图片压缩。
     * Bitmap的格式必须是ARGB_8888 [android.graphics.Bitmap.Config]。
     *
     * @param bitmap   图片数据
     * @param quality  压缩质量
     * @param desPath  压缩后存放的路径
     * @param optimize 是否使用哈夫曼算法
     * @return 结果
     */
    external fun nativeCompressBitmap(
        bitmap: Bitmap,
        quality: Int,
        desPath: String?,
    ): Int
}