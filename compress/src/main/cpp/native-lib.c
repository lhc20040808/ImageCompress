#include <jni.h>
#include <string.h>
#include "compress.h"
#include <sys/types.h>

typedef u_int8_t BYTE;

const char *jstring2String(JNIEnv *env, jstring jstr);

int generateJPEG(BYTE *data, int w, int h, jint quality, const char *name);

JNIEXPORT jstring JNICALL
Java_com_zhaodong_compress_ImageCompress_stringFromJNI(JNIEnv *env, jobject thiz) {
    char str[] = "Hello World";
    return (*env)->NewStringUTF(env, str);
}

JNIEXPORT jint JNICALL
Java_com_zhaodong_compress_ImageCompress_nativeCompressBitmap(JNIEnv *env, jobject thiz,
                                                              jobject bitmap, jint quality,
                                                              jstring des_path, jboolean optimize) {
    AndroidBitmapInfo androidBitmapInfo;
    BYTE *pixelsColor;
    int ret;
    BYTE *data;
    BYTE *tmpData;
    const char *dstFilePath = jstring2String(env, des_path);
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &androidBitmapInfo)) < 0) {
        LOGD("AndroidBitmap_getInfo() failed error=%d", ret);
        return ret;
    }
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, (void **) &pixelsColor)) < 0) {
        LOGD("AndroidBitmap_lockPixels() failed error=%d", ret);
        return ret;
    }

    int w, h, format;
    w = androidBitmapInfo.width;
    h = androidBitmapInfo.height;
    format = androidBitmapInfo.format;

    LOGD("bitmap:width=%d,height=%d,size=%d,format=%d", w, h, w * h, format);

    BYTE r, g, b;
    data = (BYTE *) malloc(w * h * 3);
    tmpData = data;
    int color;

    //去掉图片的透明通道
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if (format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
                //int占4个字节，把pixelsColor强转成指向int的指针再取值。此时取值取4个字节
                color = (*(int *) (pixelsColor));
                //根据官网RGBA_8888的公式：int color = (A & 0xff) << 24 | (B & 0xff) << 16 | (G & 0xff) << 8 | (R & 0xff);
                //反推即可取出rgb
                b = (color >> 16) & 0xFF;
                g = (color >> 8) & 0xFF;
                r = (color >> 0) & 0xFF;
                *data = r;
                *(data + 1) = g;
                *(data + 2) = b;
                data += 3;
                pixelsColor += 4;
            } else {
                return -2;
            }
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    //进行压缩
    ret = generateJPEG(tmpData, w, h, quality, dstFilePath);
    free((void *) dstFilePath);
    free((void *) tmpData);
    return ret;
}

const char *jstring2String(JNIEnv *env, jstring jstr) {
    char *ret = NULL;
    const char *tempStr = (*env)->GetStringUTFChars(env, jstr, NULL);
    jsize len = (*env)->GetStringUTFLength(env, jstr);
    if (len > 0) {
        ret = (char *) malloc(len + 1);
        memcpy(ret, tempStr, len);
        ret[len] = 0;
    }
    (*env)->ReleaseStringUTFChars(env, jstr, tempStr);
    return ret;
}

int generateJPEG(BYTE *data, int w, int h, jint quality, const char *name) {

}