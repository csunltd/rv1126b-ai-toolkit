#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>

#define BROKER_ADDRESS "localhost"  // broker アドレスを定義します
#define PORT 1883                   // broker ポート番号を定義します
#define KEEP_ALIVE 60               // keepalive 時間を定義します

struct mosquitto *mosq = NULL;

void on_connect(struct mosquitto *mosq, void *userdata, int rc) {
    if (rc) {
        printf("接続に失敗しました： %s\n", mosquitto_connack_string(rc));
    } else {
        printf("接続に成功しました\n");
    }
}

void on_disconnect(struct mosquitto *mosq, void *userdata, int rc) {
    printf("接続を切断しました\n");
}

void on_publish(struct mosquitto *mosq, void *userdata, int mid) {
    printf("メッセージ送信に成功しました，mid=%d\n", mid);
}

void on_subscribe(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos) {
    printf("購読に成功しました\n");
}

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    printf("メッセージを受信しました：\n");
    printf("トピック： %s\n", message->topic);
    printf("メッセージ： %s\n", (char*)message->payload);
}

int main(int argc, char *argv[]) {
    int rc = -1;
    char *topic01 = "test/Easy-Eai-01";
    char *topic02 = "test/Easy-Eai-02";
    int qos = 1;

    mosquitto_lib_init();   // mosquitto ライブラリを初期化します

    mosq = mosquitto_new("subsrciption_cliend", true, NULL);   // 新しい mosquitto クライアントを作成します
    if (!mosq) {
        fprintf(stderr, "クライアントの作成に失敗しました\n");
        goto cleanup;
    }

    // コールバック関数を設定します
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_disconnect_callback_set(mosq, on_disconnect);
    mosquitto_publish_callback_set(mosq, on_publish);
    mosquitto_subscribe_callback_set(mosq, on_subscribe);
    mosquitto_message_callback_set(mosq, on_message);

    // broker に接続します
    rc = mosquitto_connect(mosq, BROKER_ADDRESS, PORT, KEEP_ALIVE);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "接続に失敗しました: %s\n", mosquitto_strerror(rc));
        goto cleanup;
    }

    // トピックを購読します
    rc = mosquitto_subscribe(mosq, NULL, topic01, qos);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "購読に失敗しました: %s\n", mosquitto_strerror(rc));
        goto cleanup;
    }

    rc = mosquitto_subscribe(mosq, NULL, topic02, qos);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "購読に失敗しました: %s\n", mosquitto_strerror(rc));
        goto cleanup;
    }
    //コールバックをループ実行してメッセージを受信します
    mosquitto_loop_forever(mosq, -1, 1);

cleanup:
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return rc;

}

