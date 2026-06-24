 /**
 *
 * Copyright 2021 by Guangzhou Easy EAI Technologny Co.,Ltd.
 * website: www.easy-eai.com
 *
 * Author: Jiehao.Zhong <zhongjiehao@easy-eai.com>
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * License file for more details.
 * 
 */

//===========================================system===========================================
#include <sys/time.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/vfs.h>

#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include <termios.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <iconv.h>
#include <fcntl.h>
#include <dirent.h>
#include <dirent.h>
#include <semaphore.h>

//=========================================== C++ ===========================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//======================================= system_opt =======================================
#include "system_opt.h"

using namespace std;

/***********************************************************
    システム内の一部ではポインタへ値を代入する必要がありますが、代入前に
    ポインタが NULL かどうかを判定します。ここではマクロとして統一定義します
************************************************************/
#define TRY_EVALUATE_POINTER(pointer, value) do{\
    if(pointer)\
    {\
        *(pointer) = value;\
    }\
}while(0)


/*********************************************************************
Function:
Description:
	CPU のリアルタイム温度を取得します
Example:
	double cpuTemp = cpu_tempture();
parameter:
    なし
Return:
	CPU のリアルタイム温度。単位：摂氏度
********************************************************************/
double cpu_tempture()
{
    FILE *fd = NULL;
    int temp;
    char buff[256];

    fd = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    if(fd){
        fgets(buff,sizeof(buff),fd);
        sscanf(buff, "%d", &temp);

        fclose(fd);
        
        return (double)temp/1000.0;
    }else{
        return 0.0;
    }
}

/*********************************************************************
Function:
Description:
	NPU のリアルタイム温度を取得します
Example:
	double npuTemp = npu_tempture();
parameter:
    なし
Return:
	NPU のリアルタイム温度。単位：摂氏度
********************************************************************/
double npu_tempture()
{
    FILE *fd = NULL;
    int temp;
    char buff[256];

    fd = fopen("/sys/class/thermal/thermal_zone1/temp","r");
    if(fd){
        fgets(buff,sizeof(buff),fd);
        sscanf(buff, "%d", &temp);

        fclose(fd);
        
        return (double)temp/1000.0;
    }else{
        return 0.0;
    }
}

/*********************************************************************
Function:
Description:
	CPU 状態を cpu_occupy_t 構造体へ取得します
Example:
	get_cpu_occupy((cpu_occupy_t *)&cpu_stat1);
parameter:
    cpust: CPU 状態を格納するための構造体
Return:
	なし
********************************************************************/
void get_cpu_occupy(cpu_occupy_t *cpust)
{
    FILE *fd;
    char buff[256];
    cpu_occupy_t *cpu_occupy;

    cpu_occupy=cpust;
    fd = fopen ("/proc/stat", "r");

    if(fd == NULL){
        perror("fopen:");
        exit (0);
    }

    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %u %u %u %u %u %u %u", 
                        cpu_occupy->name,
                        &cpu_occupy->user,
                        &cpu_occupy->nice,
                        &cpu_occupy->system,
                        &cpu_occupy->idle,
                        &cpu_occupy->iowait,
                        &cpu_occupy->irq,
                        &cpu_occupy->softirq);
    fclose(fd);
}

/*********************************************************************
Function:
Description:
	新旧 2 回分の CPU 状態から CPU 使用率を計算します
Example:
	double cpu_usage()
    {
        static bool bIsFirstTimeGet = true;
        static cpu_occupy_t cpu_stat1;
        cpu_occupy_t cpu_stat2;

        double cpu;

        if(bIsFirstTimeGet){
            bIsFirstTimeGet = false;
            
            get_cpu_occupy((cpu_occupy_t *)&cpu_stat1);
            msleep(500);
            get_cpu_occupy((cpu_occupy_t *)&cpu_stat2);
        }else{
            get_cpu_occupy((cpu_occupy_t *)&cpu_stat2);
        }
        //CPU 使用率を計算します
        cpu = cal_cpu_occupy ((cpu_occupy_t *)&cpu_stat1, (cpu_occupy_t *)&cpu_stat2);
        memcpy(&cpu_stat1, &cpu_stat2, sizeof(cpu_occupy_t));

        return cpu;
    }
parameter:
    o: 前回取得した CPU 状態
    n: 新しく取得した CPU 状態
Return:
	CPU 使用率
********************************************************************/
double cal_cpu_occupy(cpu_occupy_t *o, cpu_occupy_t *n)
{
    double od, nd;
    double id, sd;
    double cpu_use;

    od = (double) (o->user + o->nice + o->system + o->idle + o->softirq + o->iowait + o->irq);//1 回目の（user + nice + system + idle）時間を od に代入します
    nd = (double) (n->user + n->nice + n->system + n->idle + n->softirq + n->iowait + n->irq);//2 回目の（user + nice + system + idle）時間を nd に代入します
    id = (double) (n->idle); //1 回目と 2 回目の idle 時間差を id に代入します
    sd = (double) (o->idle) ; //1 回目の idle 時間を sd に代入します
    
    if((nd-od) != 0)
        cpu_use =100.0 - ((id-sd))/(nd-od)*100.00; //((user + system) * 100) を 1 回目と 2 回目の時間差で割った値を g_cpu_used に代入します
    else
        cpu_use = 0;

    return cpu_use;
}

/*********************************************************************
Function:
Description:
	利用可能メモリに対する使用済み領域の使用率を確認します
Example:
	double memUsage = memory_usage();
parameter:
    なし
Return:
	利用可能メモリに対する使用済み領域の使用率
********************************************************************/
double memory_usage()
{
#if 0
    double totalRam = 0.0;
    double freeRam = 0.0;
    double usedRam = 0.0;
    struct sysinfo sysInfo;
    if(sysinfo(&sysInfo) == 0)
    {
        totalRam = (double)sysInfo.totalram;
        freeRam  = (double)(sysInfo.freeram + sysInfo.bufferram);
        usedRam = totalRam - freeRam;
        printf("total:[uint](%ld)--[double](%f)\n", sysInfo.totalram, totalRam);
        printf("free:[uint](%ld)--[double](%f)\n", sysInfo.freeram + sysInfo.bufferram, freeRam);
        printf("buff:[uint](%ld)--[double](  )\n", sysInfo.bufferram);
        printf("used:[uint](   )--[double](%f)\n", usedRam);
        printf (100*(double)usedRam/(double)totalRam);
    }
    return 100.0;
#else
    char strMem[64];
    uint64_t totalRam = 0;
    uint64_t usedRam = 0;

    memset(strMem, 0, sizeof(strMem));
    exec_cmd_by_popen("free -b | grep Mem | awk '{print $2}'", strMem);
    strMem[strlen(strMem)-1] = 0;
    totalRam = atoi(strMem);

    memset(strMem, 0, sizeof(strMem));
    exec_cmd_by_popen("free -b | grep Mem | awk '{print $3}'", strMem);
    strMem[strlen(strMem)-1] = 0;
    usedRam = atoi(strMem);

    //printf("total : %llu, used : %llu\n", totalRam, usedRam);

    return 100.0 * (double)usedRam/(double)totalRam;
#endif
}

/*********************************************************************
Function:
Description:
	マウントポイント（パーティション）の使用率を取得します
Example:
	double diskUsage = partition_usage("/userdata");
parameter:
    path: 照会対象のマウントポイント（パーティション）がファイルシステム上に存在するディレクトリ
Return:
	マウントポイント（パーティション）の使用済み領域の使用率
********************************************************************/
double partition_usage(const char *path)
{
    /*
    struct statfs 
    { 
       long    f_type;     // ファイルシステムタイプ  
       long    f_bsize;    // 最適化された転送ブロックサイズ  
       long    f_blocks;   // ファイルシステムデータブロック総数 
       long    f_bfree;    // 利用可能ブロック数
       long    f_bavail;   // 非スーパーユーザーが取得可能なブロック数 
       long    f_files;    // ファイルノード総数
       long    f_ffree;    // 利用可能ファイルノード数
       fsid_t  f_fsid;     // ファイルシステム識別子
       long    f_namelen;  // ファイル名の最大長
    };
    */
    struct statfs s;
    memset(&s, 0, sizeof(struct statfs));
    
    if(0 == statfs(path, &s)){

        double percentage = (s.f_blocks - s.f_bfree) * 100 /(s.f_blocks - s.f_bfree + s.f_bavail) + 1;
#if 0
        int64_t bsize = s.f_bsize;                // in bytes
        int64_t totalSize = (bsize * s.f_blocks);      // in bytes
        int64_t freeSize = (bsize * s.f_bfree);          // in bytes
        int64_t availSize = (bsize * s.f_bavail);         // in bytes
#endif
        return percentage;
    }else{
        return 100.0;
    }
}

/*********************************************************************
Function:
Description:
	システムタイムスタンプを取得します。通常は性能テストに使用します
Example:
	uint64_t timeval_bf = get_timeval_us();
parameter:
    なし
Return:
	システムタイムスタンプ（UTC 時間、タイムゾーン補正なし）。単位：マイクロ秒
********************************************************************/
uint64_t get_timeval_us()
{
    struct timeval tv;
	gettimeofday(&tv, NULL);	// UTC 時間
	
	return ((uint64_t)tv.tv_sec * 1000000 + tv.tv_usec);
}

/*********************************************************************
Function:
Description:
	システムタイムスタンプを取得します。通常は性能テストに使用します
Example:
	uint64_t timeval_bf = get_timeval_ms();
parameter:
    なし
Return:
	システムタイムスタンプ（UTC 時間、タイムゾーン補正なし）。単位：ミリ秒
********************************************************************/
uint64_t get_timeval_ms()
{
    struct timeval tv;
	gettimeofday(&tv, NULL);	// UTC 時間
	return ((uint64_t)tv.tv_sec * 1000 + tv.tv_usec/1000);
}

/*********************************************************************
Function:
Description:
	システムタイムスタンプを取得します。通常は性能テストに使用します
Example:
	uint64_t timeval_bf = get_timeval_s();
parameter:
    なし
Return:
	システムタイムスタンプ（UTC 時間、タイムゾーン補正なし）。単位：秒
********************************************************************/
uint64_t get_timeval_s()
{
    struct timeval tv;
	gettimeofday(&tv, NULL);	// UTC 時間
	
	return (uint64_t)tv.tv_sec;
}

/*********************************************************************
Function:
Description:
	ミリ秒単位の遅延
Example:
	osTask_usDelay(10);
parameter:
    s: 遅延時間 - 単位：マイクロ秒
Return:
	なし
説明:
	- nanosleep が呼び出されると、プロセスは TASK_INTERRUPTIBLE 状態に入り、プロセスが起床されると TASK_RUNNING 状態に戻ります。
	- TASK_INTERRUPTIBLE と TASK_UNINTERRUPTIBLE の違い：
		TASK_INTERRUPTIBLE は［シグナル］および［wake_up()］によって起床できます。シグナルが到達すると、プロセスは実行可能状態に設定されます。
		TASK_UNINTERRUPTIBLE は [wake_up()] によってのみ起床できます。
********************************************************************/
uint32_t osTask_usDelay(uint32_t us)
{
	uint32_t elaTime;
	struct timespec delayTime, elaspedTime;

	delayTime.tv_sec  = us / 1000000;
	delayTime.tv_nsec = (us % 1000000) * 1000;

	nanosleep(&delayTime, &elaspedTime);

	elaTime = (elaspedTime.tv_sec*1000000 + elaspedTime.tv_nsec/1000);
	return elaTime;
}

uint32_t osTask_msDelay(uint32_t ms)
{
	uint32_t elaTime;
	struct timespec delayTime, elaspedTime;

	delayTime.tv_sec  = ms / 1000;
	delayTime.tv_nsec = (ms % 1000) * 1000000;

	nanosleep(&delayTime, &elaspedTime);

	elaTime = (elaspedTime.tv_sec*1000 + elaspedTime.tv_nsec/1000000);
	return elaTime;
}

uint32_t osTask_sDelay(uint32_t s)
{
	uint32_t elaTime;
	struct timespec delayTime, elaspedTime;

	delayTime.tv_sec  = s;
	delayTime.tv_nsec = 0;

	nanosleep(&delayTime, &elaspedTime);

	elaTime = elaspedTime.tv_sec;
	return elaTime;
}

/*********************************************************************
Function:
Description:
	ミリ秒単位の遅延
Example:
	msleep(10);
parameter:
    ms: 遅延時間 - 単位：ミリ秒
Return:
	プロセス／スレッドが割り込まれた場合、残り時間（ms）を返します
********************************************************************/
uint32_t msleep (uint32_t ms)
{
	uint32_t ret = usleep(ms*1000);
	if(ret)
		return (ret/1000);
	else
		return 0;
}

/*********************************************************************
Function:
Description:
	システムタイムスタンプを取得します
Example:
	int timeStamp = get_time_stamp();
parameter:
	なし
Return:
	現在のシステムタイムスタンプ（UTC 時間、タイムゾーン補正なし）
********************************************************************/
int32_t get_time_stamp()
{
	time_t t;
	t = time(NULL);	// UTC 時間
 
	return time(&t);
}

/*********************************************************************
Function:
Description:
	日付と時刻を年・月・日・時・分・秒の順にパラメータへ格納します
Example:
	uint32_t curDate, curTime
	get_system_date_time(&curDate, &curTime);
parameter:
    *curDate:現在日期
    *curTime: 現在時刻（タイムゾーン補正済み）
Return:
	なし
********************************************************************/
void get_system_date_time(uint32_t *curDate, uint32_t *curTime)
{
    time_t timer;//time_t就是long int タイプ
    struct tm *tblock;

    timer = time(NULL);	// UTC 時間
    tblock = localtime(&timer); //ローカルタイムゾーンの時刻を取得します

	*curDate = 10000 * (tblock->tm_year+1900) + 100 * (tblock->tm_mon + 1) + (tblock->tm_mday);
 	*curTime = 10000 * (tblock->tm_hour) + 100 * (tblock->tm_min) + (tblock->tm_sec);
}

/*********************************************************************
Function:
Description:
	システム時刻を設定します。自動時刻補正に注意してください
Example:
	set_system_date_time(2021, 12, 22, 11, 14, 59);
parameter:
    year：年
     mon：月
     day：日
    hour: 時（現在のタイムゾーン時刻。追加のタイムゾーン補正は不要です）
     min：分
    second：秒
Return:
	なし
********************************************************************/
void set_system_date_time(int year, int mon, int day, int hour, int min, int second)
{
	time_t tStamp;
    struct timeval tv;

    struct tm t;
	
    year -= 1900;
    t.tm_year = year;
    t.tm_mon = mon-1;
    t.tm_mday = day;

    t.tm_hour = hour;	// ローカルタイムゾーンの時刻を設定します
    t.tm_min = min;
    t.tm_sec = second;

	tStamp = mktime(&t); // mktime の戻り値は UTC 時間です
    if(-1 == tStamp){
        perror("mktime");
    }else{
		tv.tv_sec = tStamp;
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);	// UTC 時間
	}
}

/*********************************************************************
Function:
Description:
	計算当天是星期几
Example:
	calc_week_day(2022, 3, 18);
parameter:
	y:年
	m:月
	d:日
Return:
	星期几(0-月曜日；6-日曜日)
********************************************************************/
uint8_t calc_week_day(int y,int m, int d)
{
    uint8_t iWeek;
    if(m==1||m==2) {
        m+=12;
        y--;
    }
    iWeek=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;
#ifdef __TEST_SOFT__
    switch(iWeek)
    {
        case 0: printf("月曜日\n"); break;
        case 1: printf("火曜日\n"); break;
        case 2: printf("水曜日\n"); break;
        case 3: printf("木曜日\n"); break;
        case 4: printf("金曜日\n"); break;
        case 5: printf("土曜日\n"); break;
        case 6: printf("日曜日\n"); break;
    }
#endif
    return iWeek;
}


/*********************************************************************
Function:
Description:
	分離モードでスレッドを作成します。新しいスレッドがメインスレッドと変数を共有する場合、そのスレッドは必ず確認する必要があります
	実行中にスレッド本体で使用する変数がメインスレッドによって解放されていないこと。そうしないと問題が発生します。
Example:
    void *xxxThreadBody(void *arg)
	{
		pthread_exit(NULL);
	}
	
	pthread_t pId;
	int share_para;
	CreateNormalThread(xxxThreadBody, &share_para, &pId);
parameter:
    entry: スレッド本体の実行関数
    *para: スレッド本体へ渡すパラメータ。共有変数として使用します
    *pid: 渡された pid が NULL の場合、プロセス全体を直接終了します
Return:
	 0：作成成功
	-1：作成失敗
********************************************************************/
int32_t CreateNormalThread(ThreadEntryPtrType entry, void *para, pthread_t *pid)
{
    pthread_t ThreadId;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);// バインド
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);// デタッチ
    if(pthread_create(&ThreadId, &attr, entry, para) == 0)//スレッドを作成します
    {
        pthread_attr_destroy(&attr);
        TRY_EVALUATE_POINTER(pid, ThreadId);

        return 0;
    }

    pthread_attr_destroy(&attr);

    return -1;
}

int32_t CreateJoinThread(ThreadEntryPtrType entry, void *para, pthread_t *pid)
{
    pthread_t ThreadId;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if(pthread_create(&ThreadId, &attr, entry, para) == 0)
    {
        pthread_attr_destroy(&attr);
        TRY_EVALUATE_POINTER(pid, ThreadId);

        return 0;
    }

    pthread_attr_destroy(&attr);

    return -1;
}

int32_t WaitExitThread(pthread_t pid)
{
    if (pthread_join(pid, NULL) == 0){
        return 0;
    }
    return -1;
}

/*********************************************************************
Function:
Description:
	シェルコマンドを実行します
Example:
	SYSTEM("ls");
parameter:
	*cmdstring: シェルコマンド文
Return:
	fork が失敗した場合、system() 関数は -1 を返します。
	exec の実行に成功し、command が正常に完了した場合、command が exit または return で返した値を返します。
	(注意：command が正常に実行されたことは、処理が成功したことを意味しません。例：command が "rm debuglog.txt" の場合、ファイルが存在するかどうかにかかわらず command 自体は正常に実行されます)
	exec の実行に失敗した場合、つまり command が正常に実行されなかった場合（シグナルで中断された、または command コマンド自体が存在しないなど）、system() 関数は 127 を返します。
注意：
	* system() または SYSTEM() の呼び出し時にシグナル処理方式が SIG_DFL でない場合、親プロセスは waitpid() 関数で子プロセスを回収できません。その結果、ゾンビプロセスが発生します。
	* wait() または waitpid() を使用して子プロセスを回収したい場合は、呼び出し前（実際には fork() 前）に SIGCHLD シグナルを SIG_DFL 処理方式へ設定する必要があります。
	  呼び出し後（実際には wait()/waitpid() 後）に、シグナル処理方式を以前の値へ戻します。
呼び出し時の推奨事項：
	1、system() 関数はシェルコマンド実行のみに使用することを推奨します。一般に system()/SYSTEM() の戻り値が 0 でない場合はエラーを示します。
	2、system()/SYSTEM() 関数実行後の errno 値を監視し、エラー時により有用な情報を出力することを推奨します。
	3、waitpid() 関数がシグナル割り込みにより負の値を返した場合は、waitpid() 関数を再度呼び出します。これは SIGINT も含み、POSIX.1 定義に違反しません。
	4、system()/SYSTEM() の非ブロッキング方式に関する注意点：`&` でバックグラウンド化し、同時に出力をリダイレクトしてください。そうしないとブロッキング方式になります。
	5、system()/SYSTEM() 関数の代替として popen() の利用を検討することを推奨します。
********************************************************************/
static int32_t SYSTEM(const char *cmdstring)
{
    pid_t pid;
    int status;

    if(cmdstring == NULL)
    {
        return (1);
    }

	// この関数とシステムの system() との差異は、システムは fork() を使用し、この関数は vfork() を使用する点です
	// fork()  -子プロセスは親プロセスのデータセグメントとコードセグメントをコピーします。ここではページテーブルのコピーにより実現されます。
	//         -親子プロセスの実行順序は不定です。
	// vfork() -子プロセスは親プロセスとアドレス空間を共有するため、ページテーブルをコピーする必要がなく、効率が高くなります。
	//         -子プロセスが先に実行されることを保証し、exec または exit を呼び出すまでは親プロセスとデータを共有します。親プロセスは子プロセスが exec または exit を呼び出した後にのみスケジューリングされる可能性があります。これらの関数を呼び出す前に子プロセスが親プロセスの追加処理に依存している場合、デッドロックが発生します。
    if((pid = vfork())<0)
    {
        status = -1;
    }
    else if(pid == 0)	//子プロセスでスクリプトコマンドを実行します
    {
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        exit(127); //子プロセスが正常に実行された場合、この文は実行されません
    }
    else		//親プロセスで子プロセスの戻りを対象機します
    {
        while(waitpid(pid, &status, 0) < 0)
        {
            if(errno != EINTR)
            {
                status = -1;
                break;
            }
        }
    }
    return status;
}

/*********************************************************************
Function:
Description:
	シェルコマンドを実行します
Example:
	exec_cmd_by_system("ls");
parameter:
	*cmd: シェルコマンド文
Return:
	同SYSTEM(const char *cmdstring);
注意：
	SIGCHLD のシグナル動作が SIG_IGN に設定されている場合、waitpid() 関数は子プロセスを見つけられず ECHILD エラーを返す可能性があります。
	解析：
    * systeme() 関数は、カーネルがプロセス初期化時に SIGCHLD シグナルの処理方式を SIG_DFL にするというシステム特性に依存しています。
	
	-[シグナルの処理方式が SIG_DFL]とはどういう意味でしょうか。
	* つまり、カーネルはプロセスの子プロセス終了を検出すると SIGCHLD シグナルをプロセスへ送信し、プロセスはそのシグナルを SIG_DFL 方式で処理します。
	
	-では SIG_DFL とはどのような方式でしょうか。
	* SIG_DFL は、シグナル処理関数ポインタを定義するマクロです。実際にはこのシグナル処理関数は何もしません。この特性こそが system() 関数に必要なものです。
	* system() 関数はまず fork() で子プロセスを生成して command コマンドを実行し、実行完了後に waitpid() 関数で子プロセスを回収します。	
********************************************************************/
int32_t exec_cmd_by_system(const char *cmd)
{
	int32_t ret = 0;
	sighandler_t old_handler;

	old_handler = signal(SIGCHLD, SIG_DFL);	//ゾンビプロセスの発生を防止します
	ret = SYSTEM(cmd);
	signal(SIGCHLD, old_handler);

	return ret;
}

/*********************************************************************
Function:
Description:
	シェルコマンドを実行します
Example:
	char result[1024]={0};
	exec_cmd_by_popen("ls", result);
parameter:
	*cmd: シェルコマンド文
	*result: シェルコマンド実行後の結果をこのメモリ領域に格納します
Return:
	なし
注意：
	- cmd の実行に失敗した場合、子プロセスはエラー情報を標準エラー出力へ出力するため、親プロセスは取得できません。
	  エラー情報を取得する必要がある場合は、子プロセスのエラー出力を標準出力へリダイレクト（2>&1）してください。これにより親プロセスが子プロセスのエラー情報を取得できます。
	  例：exec_cmd_by_popen("ls 2>&1", result);
********************************************************************/
int32_t exec_cmd_by_popen(const char *cmd, char *result)
{
    char buf_ps[1024];
    char ps[1024]={0};
    FILE *ptr;
    strcpy(ps, cmd);
    if((ptr=popen(ps, "r"))!=NULL){
        while(fgets(buf_ps, 1024, ptr)!=NULL)
        {
//	       この行を使用して、シェルコマンド出力の各行を取得できます
//	   	   printf("%s", buf_ps);
           strcat(result, buf_ps);
           if(strlen(result)>1024)
               break;
        }
        pclose(ptr);
        ptr = NULL;
		return 0;
    } else {
        printf("popen %s error\n", ps);
		return -1;
    }
}

