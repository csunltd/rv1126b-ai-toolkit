#include <stdio.h>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

#include "jpegCodec.h"

typedef struct {
    GstElement *pipeline;
    GstElement *appsrc;
} AppData;

AppData g_decpipe_obj={0};

static GstFlowReturn new_sample(GstElement *appsink, OutDataCB pOutFunc) {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));
    if (sample) {
        GstBuffer *buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        
        if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
            // デコード後の生映像データを取得します
            GstCaps *caps = gst_sample_get_caps(sample);
            GstStructure *structure = gst_caps_get_structure(caps, 0);

            SrcDataDesc_t dataDesc;
            const gchar *format = gst_structure_get_string(structure, "format");
            strcpy(dataDesc.fmt, format);
            gst_structure_get_int(structure, "width", &dataDesc.width);
            gst_structure_get_int(structure, "height", &dataDesc.height);
            dataDesc.horStride = dataDesc.width;
            dataDesc.verStride = dataDesc.height;
            dataDesc.dataSize = map.size;
            //g_print("Decoded frame: %dx%d, format: %s, size: %zu\n", dataDesc.width, dataDesc.height, format, map.size);
            
            /* ここで生映像データを処理できます：
             * map.data ピクセルデータを含みます
             * 実際の形式に応じて(RGB/YUV等)データを処理します
             */
            /* ここで生データ（RGB/YUV）をコールバックへ渡して処理します：
             * map.data ピクセルデータを含みます
             * dataDesc は生データに関する情報記述です
             */
            pOutFunc((void *)map.data, dataDesc);
            
            gst_buffer_unmap(buffer, &map);
        }
        gst_sample_unref(sample);
    }
    return GST_FLOW_OK;
}

int JpegDec_init(const char *strDecoder, OutDataCB pOutFunc)
{
    //gst_init(&argc, &argv);
    gst_init(NULL, NULL);
    
    if(NULL != g_decpipe_obj.pipeline){
        JpegDec_unInit();
    }

    // 1.1--要素を作成します
    g_decpipe_obj.pipeline = gst_pipeline_new("jpeg-decoder");
    g_decpipe_obj.appsrc = gst_element_factory_make("appsrc", "source");
    GstElement *jpegparse = gst_element_factory_make("jpegparse", "jpegparse");
    GstElement *decoder = gst_element_factory_make(strDecoder, "decoder");
    GstElement *appsink = gst_element_factory_make("appsink", "sink");

    if (!g_decpipe_obj.pipeline || !g_decpipe_obj.appsrc || 
        !jpegparse || !decoder || !appsink) {
        g_printerr("Failed to create elements\n");
        return -1;
    }
    //=========================================================================================

    // 1.2--appsrc を設定します：JPEG デコードパイプラインの入力として使用します
    GstCaps *jpeg_caps = gst_caps_new_simple("image/jpeg", NULL, NULL);
    g_object_set(g_decpipe_obj.appsrc,
                "caps", jpeg_caps,
                "stream-type", GST_APP_STREAM_TYPE_STREAM, 
                "format", GST_FORMAT_TIME,
                "block", TRUE,          // ブロッキングモード
                "do-timestamp", TRUE,   // 自動タイムスタンプ（カスタムしない場合）
                NULL);
    gst_caps_unref(jpeg_caps);
    //=========================================================================================

    // 1.3--appsink を設定します：JPEG デコードパイプラインの出力として使用します
    g_object_set(appsink,
                "emit-signals", TRUE,
                "sync", FALSE,
                NULL);
    g_signal_connect(appsink, "new-sample", G_CALLBACK(new_sample), pOutFunc);
    //=========================================================================================

    // 2--パイプラインを構築します
    gst_bin_add_many(GST_BIN(g_decpipe_obj.pipeline), g_decpipe_obj.appsrc, jpegparse, decoder, appsink, NULL);
    if (!gst_element_link_many(g_decpipe_obj.appsrc, jpegparse, decoder, appsink, NULL)) {
        g_printerr("Failed to link elements\n");
        return -1;
    }
    //=========================================================================================

    return 0;
}


void JpegDec_start()
{
    // 1--パイプラインを開始します
    gst_element_set_state(g_decpipe_obj.pipeline, GST_STATE_PLAYING);
    //=========================================================================================

    // 2--メッセージループ
    GstBus *bus = gst_element_get_bus(g_decpipe_obj.pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // 3--リソースをクリーンアップします
    if(msg)
        gst_message_unref(msg);
    
    gst_object_unref(bus);
    //=========================================================================================

    return ;
}


// JPEG 形式データはここから JPEG デコーダへ入力する必要があります。
int JpegDec_pushData(char *pJpegData, int dataSize, int isEOS)
{
    if ((NULL == pJpegData) || (dataSize <= 0)) {
        // EOS 終了フラグを送信します
        gst_app_src_end_of_stream(GST_APP_SRC(g_decpipe_obj.appsrc));
        return G_SOURCE_REMOVE;
    }
    
    GstBuffer *buffer = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, pJpegData, dataSize,
                                         0, // offset
                                         dataSize,
                                         NULL, // user_data
                                         NULL); // notify
    
    // タイムスタンプを追加します（任意）
    // static guint frame_count = 0;
    // GST_BUFFER_PTS(buffer) = gst_util_uint64_scale(frame_count, GST_SECOND, 30); // 30fps と仮定します
    // GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale(1, GST_SECOND, 30);
    
    GstFlowReturn flow = gst_app_src_push_buffer(GST_APP_SRC(g_decpipe_obj.appsrc), buffer);
    if (flow != GST_FLOW_OK) {
        g_printerr("Push buffer error: %d\n", flow);
        return G_SOURCE_REMOVE;
    }
    
    // EOS を送信します
    if(isEOS){
        gst_app_src_end_of_stream(GST_APP_SRC(g_decpipe_obj.appsrc));
    }

    return G_SOURCE_CONTINUE;
}

int JpegDec_unInit()
{
    gst_element_set_state(g_decpipe_obj.pipeline, GST_STATE_NULL);
    gst_object_unref(g_decpipe_obj.pipeline);

    return 0;
}
