#include <jni.h>
#include <string>


extern "C"
JNIEXPORT jstring JNICALL
Java_com_zhaodong_compress_ImageCompress_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}