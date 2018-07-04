#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <jni.h>
#include <FFmpeg/Utils/logHelp.h>

static const char *type_string(int type)
{
    switch (type) {
        case AVIO_ENTRY_DIRECTORY:
            return "<DIR>";
        case AVIO_ENTRY_FILE:
            return "<FILE>";
        case AVIO_ENTRY_BLOCK_DEVICE:
            return "<BLOCK DEVICE>";
        case AVIO_ENTRY_CHARACTER_DEVICE:
            return "<CHARACTER DEVICE>";
        case AVIO_ENTRY_NAMED_PIPE:
            return "<PIPE>";
        case AVIO_ENTRY_SYMBOLIC_LINK:
            return "<LINK>";
        case AVIO_ENTRY_SOCKET:
            return "<SOCKET>";
        case AVIO_ENTRY_SERVER:
            return "<SERVER>";
        case AVIO_ENTRY_SHARE:
            return "<SHARE>";
        case AVIO_ENTRY_WORKGROUP:
            return "<WORKGROUP>";
        case AVIO_ENTRY_UNKNOWN:
        default:
            break;
    }
    return "<UNKNOWN>";
}

static int list_op(const char *input_dir)
{
    AVIODirEntry *entry = NULL;
    AVIODirContext *ctx = NULL;
    int cnt, ret;
    char filemode[4], uid_and_gid[20];

    if ((ret = avio_open_dir(&ctx, input_dir, NULL)) < 0) {
        LOGE("Cannot open directory: %s.\n", av_err2str(ret));
        goto fail;
    }

    cnt = 0;
    for (;;) {
        if ((ret = avio_read_dir(ctx, &entry)) < 0) {
            LOGE("Cannot list directory: %s.\n", av_err2str(ret));
            goto fail;
        }
        if (!entry)
            break;
        if (entry->filemode == -1) {
            snprintf(filemode, 4, "???");
        } else {
            snprintf(filemode, 4, "%3"PRIo64, entry->filemode);
        }
        snprintf(uid_and_gid, 20, "%"PRId64"(%"PRId64")", entry->user_id, entry->group_id);
        if (cnt == 0)
            LOGE("%-9s %12s %30s %10s %s %16s %16s %16s\n",
                   "TYPE", "SIZE", "NAME", "UID(GID)", "UGO", "MODIFIED",
                   "ACCESSED", "STATUS_CHANGED");
        LOGE("%-9s %12"PRId64" %30s %10s %s %16"PRId64" %16"PRId64" %16"PRId64"\n",
               type_string(entry->type),
               entry->size,
               entry->name,
               uid_and_gid,
               filemode,
               entry->modification_timestamp,
               entry->access_timestamp,
               entry->status_change_timestamp);
        avio_free_directory_entry(&entry);
        cnt++;
    };

    fail:
    avio_close_dir(&ctx);
    return ret;
}

static int del_op(const char *url)
{
    int ret = avpriv_io_delete(url);
    if (ret < 0)
        LOGE("Cannot delete '%s': %s.\n", url, av_err2str(ret));
    return ret;
}

static int move_op(const char *src, const char *dst)
{
    int ret = avpriv_io_move(src, dst);
    if (ret < 0)
        LOGE("Cannot move '%s' into '%s': %s.\n", src, dst, av_err2str(ret));
    return ret;
}


static void usage(const char *program_name)
{
    fprintf(stderr, "usage: %s OPERATION entry1 [entry2]\n"
                    "API example program to show how to manipulate resources "
                    "accessed through AVIOContext.\n"
                    "OPERATIONS:\n"
                    "list      list content of the directory\n"
                    "move      rename content in directory\n"
                    "del       delete content in directory\n",
            program_name);
}

JNIEXPORT void JNICALL
Java_com_leachchen_testffmpeg_MainActivity_test(JNIEnv *env, jobject instance) {

    avformat_network_init();

/*   list_op(argv[2]);
    del_op(argv[2]);
    move_op(argv[2], argv[3]);*/


    list_op("/sdcard/android/");

    avformat_network_deinit();
}