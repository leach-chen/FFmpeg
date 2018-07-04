#include <jni.h>
#include "../FFmpeg/Utils/logHelp.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"


JNIEXPORT jstring JNICALL
Java_com_example_testffmpeg_MainActivity_test(JNIEnv *env, jobject instance, jstring str) {
    char *st = (*env)->GetStringUTFChars(env,str,0);
    char *newStr = "-->this is jni value";
    char * result = malloc(sizeof(st)+ sizeof(newStr)+2);
    strcpy(result,st);
    strcat(result, newStr);
    //(*env)->ReleaseStringUTFChars(env, st, str);
    char info[10000] = {0};
    sprintf(info,"%s",avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}


JNIEXPORT jstring JNICALL
Java_com_example_testffmpeg_MainActivity_getVideoInfo(JNIEnv *env, jobject instance,jstring filePath_) {
    const char *filePath = (*env)->GetStringUTFChars(env, filePath_, 0);
    char info[10000] = {0};
    avcodec_register_all();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if(avformat_open_input(&avFormatContext,filePath,NULL,NULL));
    {
        sprintf(info,"%s","open video error!");
    }



    (*env)->ReleaseStringUTFChars(env, filePath_, filePath);
    return (*env)->NewStringUTF(env, info);
}