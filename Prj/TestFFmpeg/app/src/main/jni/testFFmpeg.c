#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <jni.h>
#include <FFmpeg/Utils/logHelp.h>

/*
error: 'for' loop initial declarations are only allowed in C99 or C11 mode
note: use option -std=c99, -std=gnu99, -std=c11 or -std=gnu11 to compile your code

LOCAL_CFLAGS       := -DANDROID -D__STDC_CONSTANT_MACROS -std=gnu++11
LOCAL_CPPFLAGS       := -w -fexceptions -frtti
LOCAL_CPPFLAGS          += -std=c++11

 */

JNIEXPORT jstring JNICALL
Java_com_leachchen_testffmpeg_MainActivity_getVideoInfo(JNIEnv *env, jobject instance,
                                                        jstring videoPath_) {
    const char *videoPath = (*env)->GetStringUTFChars(env, videoPath_, 0);
    jstring str = (*env)->NewStringUTF(env, videoPath);
    //(*env)->ReleaseStringUTFChars(env, videoPath_, videoPath);

    AVFormatContext *avFormatContext;   //格式信息结构体
    AVCodecContext *avCodecContext; //编解码信息结构体
    av_register_all(); //注册所有组件

    if (avformat_open_input(&avFormatContext, videoPath, NULL, NULL) != 0)   //打开输入视频文件
    {
        LOGE("open video fail!");
        return (*env)->NewStringUTF(env,"open video fail!");
    }

    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("can't find stream info!");
        return (*env)->NewStringUTF(env,"can't find stream info!");
    }

    int videoIndex = -1;
    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)        //为视频流类型
        {
            videoIndex = i;
            break;
        }
    }

    if(videoIndex == -1)
    {
        LOGE("can't find video stream info!");
        return (*env)->NewStringUTF(env,"can't find video stream info!");
    }

    avCodecContext = avFormatContext->streams[videoIndex]->codec;
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id); //找到解码器
    if(avCodec == NULL)
    {
        LOGE("can't find video decoder!");
        return (*env)->NewStringUTF(env,"can't find video decoder!");
    }

    if(avcodec_open2(avCodecContext,avCodec,NULL) < 0)
    {
        LOGE("can't find open codec!");
        return (*env)->NewStringUTF(env,"can't find open codec!");
    }


    int autioIndex = -1;
    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)        //为视频流类型
        {
            autioIndex = i;
            break;
        }
    }

    if(autioIndex == -1)
    {
        LOGE("can't find audio stream info!");
        return (*env)->NewStringUTF(env,"can't find audio stream info!");
    }

    char  *stt = "aaaaaa";
    char buf[100] = {0};
    sprintf(buf,"%s",stt);

    return (*env)->NewStringUTF(env,&buf);
}