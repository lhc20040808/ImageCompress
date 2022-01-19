#include <jni.h>
#include <string.h>
#include <sys/types.h>

#include "lang.h"
#include <stdlib.h>
#include <android/bitmap.h>
#include <setjmp.h>
#include <jpeglib.h>

#define COMPRESS_OPEN_DST_FILE_ERROR -3
#define COMPRESS_DEL_ALPHA_FAIL -2
#define COMPRESS_SET_JMP_FAIL -1
#define COMPRESS_SUCCESS 1

typedef u_int8_t BYTE;

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

METHODDEF(void)
jpeg_error_exit(j_common_ptr
                cinfo) {
    struct my_error_mgr *myerr = (struct my_error_mgr *) cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    LOGW("jpeg_message_table[%d]:%s",
         myerr->pub.msg_code, myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
    longjmp(myerr->setjmp_buffer, 1);
}

const char *jstring2String(JNIEnv *env, jstring jstr);

int generateJPEG(BYTE *data, int w, int h, jint quality, const char *name, boolean optimize);

JNIEXPORT jstring JNICALL
Java_com_zhaodong_compress_ImageCompress_stringFromJNI(JNIEnv *env, jobject thiz) {
    char str[] = "Hello World C++";
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
                return COMPRESS_DEL_ALPHA_FAIL;
            }
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    //进行压缩
    ret = generateJPEG(tmpData, w, h, quality, dstFilePath, true);
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

int generateJPEG(BYTE *data, int w, int h, jint quality, const char *name, boolean optimize) {
    struct jpeg_compress_struct jcs;
    struct my_error_mgr jem;

    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = jpeg_error_exit;

    if (setjmp(jem.setjmp_buffer)) {
        return COMPRESS_SET_JMP_FAIL;
    }
    //初始化jpeg对象
    jpeg_create_compress(&jcs);

    FILE *file = fopen(name, "wb");
    if (file == NULL) {
        return COMPRESS_OPEN_DST_FILE_ERROR;
    }
    jpeg_stdio_dest(&jcs, file);
    jcs.image_width = w;
    jcs.image_height = h;
    jcs.arith_code = false;
    jcs.input_components = 3;
    jcs.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jcs);
    //是否使用哈夫曼编码 0不使用哈夫曼编码 1使用哈夫曼编码
    jcs.optimize_coding = optimize;
    //设定压缩参数
    jpeg_set_quality(&jcs, quality, true);
    //开始压缩
    jpeg_start_compress(&jcs, true);

    JSAMPROW row_point[1];
    int row_stride = jcs.image_width * 3;
    while (jcs.next_scanline < jcs.image_height) {
        row_point[0] = &data[jcs.next_scanline * row_stride];
        jpeg_write_scanlines(&jcs, row_point, 1);
    }

    //压缩完毕
    jpeg_finish_compress(&jcs);
    //释放资源
    jpeg_destroy_compress(&jcs);
    fclose(file);
    return COMPRESS_SUCCESS;
}