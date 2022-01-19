#include <jni.h>
#include <string.h>
#include "compress.h"
#include <sys/types.h>

typedef u_int8_t BYTE;

//const char *jstring2String(JNIEnv *env, jstring jstr);

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

//    const char *dstFilePath = jstring2String(env, des_path);
}

//const char *jstringToString(JNIEnv *env, jstring jstr) {
//    char *ret;
//    const char *tempStr = (*env)->GetStringUTFChars(env, jstr, NULL);
//    jsize len = (*env)->GetStringUTFLength(env, jstr);
//    if (len > 0) {
//        ret = (char *) malloc(len + 1);
//        memcpy(ret, tempStr, len);
//        ret[len] = 0;
//    }
//    (*env)->ReleaseStringUTFChars(env, jstr, tempStr);
//    return ret;
//}