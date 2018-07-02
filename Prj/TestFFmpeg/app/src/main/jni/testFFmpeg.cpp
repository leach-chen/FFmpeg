#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <Utils/logHelp.h>
#include<string>
#include<iostream>

using namespace std;


extern "C" {
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#endif
}


string itos(int i) {
    string str = "";
    char n[50];
    sprintf(n, "%d", i);
    str = n;
    return str;
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_testffmpeg_MainActivity_test(JNIEnv *env, jobject instance, jstring str) {
    char *st = (char *) env->GetStringUTFChars(str, 0);
    char *newStr = "-->this is jni value";
    char *result = (char *) malloc(sizeof(st) + sizeof(newStr) + 2);
    strcpy(result, st);
    strcat(result, newStr);
    //(*env)->ReleaseStringUTFChars(env, st, str);
    char info[10000] = {0};
    sprintf(info, "%s", avcodec_configuration());
    return env->NewStringUTF(info);
}


void updateInfo(JNIEnv *env, jobject instance,string info)
{
    jclass classz = env->GetObjectClass(instance);
    jstring str = env->NewStringUTF(info.data());
    jmethodID methodId = env->GetMethodID(classz,"updateInfo","(Ljava/lang/String;)V");
    env->CallVoidMethod(instance,methodId,str);

    env->DeleteLocalRef(classz);
    env->DeleteLocalRef(str);
}

string getVideoTime(AVFormatContext *avFormatContext)
{
    char time[100] = {0};
    if(avFormatContext->duration != AV_NOPTS_VALUE){
        int hours, mins, secs, us;
        int64_t duration = avFormatContext->duration + 5000;
        secs = duration / AV_TIME_BASE;
        us = duration % AV_TIME_BASE;
        mins = secs / 60;
        secs %= 60;
        hours = mins/ 60;
        mins %= 60;
        sprintf(time,"%02d:%02d:%02d.%02d", hours, mins, secs, (100 * us) / AV_TIME_BASE);
    }
    return time;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_testffmpeg_MainActivity_getVideoInfo(JNIEnv *env, jobject instance,
                                                      jstring filePath_) {
    string info = "";
    const char *filePath = (char *) env->GetStringUTFChars(filePath_, 0);
    av_register_all();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, filePath, NULL, NULL) != 0) {
        info += "open video error!";
        env->ReleaseStringUTFChars(filePath_, filePath);
        return env->NewStringUTF(info.data());
    }

    info = info + "filePath:" + avFormatContext->filename + "\r\n";
    info = info + "time:" + getVideoTime(avFormatContext) + "\r\n";
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        info += "can't find stream info!\r\n";
        env->ReleaseStringUTFChars(filePath_, filePath);
        return env->NewStringUTF(info.data());
    }
    info = info + "streamCount:" + itos(avFormatContext->nb_streams) + "\r\n";

    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            info += "stream video type:AVMEDIA_TYPE_VIDEO\r\n";
            videoStreamIndex = i;
        } else if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            info += "stream audio type:AVMEDIA_TYPE_AUDIO\r\n";
            audioStreamIndex = i;
        }
    }

    if (videoStreamIndex < 0) {
        info += "can't find video stream!\r\n";
        env->ReleaseStringUTFChars(filePath_, filePath);
        return env->NewStringUTF(info.data());
    }

    AVCodecContext *avCodecContext = avFormatContext->streams[videoStreamIndex]->codec;

    int width = avCodecContext->width;
    int height = avCodecContext->height;

    info += "video width:" + itos(width) + "\r\n";
    info += "video height:" + itos(height) + "\r\n";

    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        info += "codec can't find\r\n";
        env->ReleaseStringUTFChars(filePath_, filePath);
        return env->NewStringUTF(info.data());
    }

    if (avcodec_open2(avCodecContext, avCodec, NULL) != 0) {
        info += "can't open codec\r\n";
        env->ReleaseStringUTFChars(filePath_, filePath);
        return env->NewStringUTF(info.data());
    }

    AVFrame *srcAVFrame = av_frame_alloc();
    AVFrame *destAVFrame = av_frame_alloc();
    if (srcAVFrame == NULL || destAVFrame == NULL) {
        info += "can't allocate video frame\r\n";
        env->ReleaseStringUTFChars(filePath_, filePath);
        return env->NewStringUTF(info.data());
    }
    int destDataCount = av_image_get_buffer_size(avCodecContext->pix_fmt, width, height,
                                                 WINDOW_FORMAT_RGBA_8888);
    info += "the data convert picture need count:" + itos(destDataCount) + "\r\n";
    uint8_t *buff = (uint8_t *) av_malloc(destDataCount * (sizeof(uint8_t)));
    av_image_fill_arrays(destAVFrame->data, destAVFrame->linesize, buff, AV_PIX_FMT_RGBA, width,
                         height, 1);
    struct SwsContext *sws_ctx = sws_getContext(width, height, avCodecContext->pix_fmt, width,
                                                height, AV_PIX_FMT_RGBA, SWS_BILINEAR, NULL, NULL,
                                                NULL);

    int isFinished;
    AVPacket avPacket;
    while (av_read_frame(avFormatContext, &avPacket) >= 0) {
        if (avPacket.stream_index == videoStreamIndex) {
            avcodec_decode_video2(avCodecContext,srcAVFrame,&isFinished,&avPacket);
            if(isFinished)
            {
                //sws_scale(sws_ctx,(uint8_t const * const *)srcAVFrame->data,srcAVFrame->linesize,0,height,destAVFrame->data,destAVFrame->linesize);
                info += "time pts:" + itos(avPacket.pts) + "\r\n";
                AVStream *stream=avFormatContext->streams[avPacket.stream_index];
                int frame_rate=stream->avg_frame_rate.num/stream->avg_frame_rate.den;
                info += "time fps:" + itos(frame_rate) + "\r\n";
                updateInfo(env,instance,info);
            }
        } else if (avPacket.stream_index == audioStreamIndex) {

        }
        av_packet_unref(&avPacket);
    }
    env->ReleaseStringUTFChars(filePath_, filePath);
    return env->NewStringUTF(info.data());
}
