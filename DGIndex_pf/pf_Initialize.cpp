#include "global.h"


//
//パイプ処理の初期化
//parse_cliを処理した直後に実行する。
//
int Initialize_pf()
{
  //initialize
  IsClosed_stdin = true;
  GetExtraData_fromStdin = false;

  if (Mode_Stdin && Initialize_stdin() == 1)
  {
    //エラー
    remove(StdinHeadFile_Path);
    return 1;                            //プロセス終了
  }

  timeFlushD2VFile = time(NULL);
  tickReadSize_speedlimit = 0;
  tickBeginTime_speedlimit = system_clock::now();
  SpeedLimit = SpeedLimit_CmdLine * 1024 * 1024;

  //デバッグ
  Enable_pfLog = false;    //　true  false

  pf_gop_Log[0] = '\0';

  min_gop_idx = INT32_MAX;
  max_gop_idx = INT32_MIN;

  return 0;
}


//
//Initialize_stdin
//
int Initialize_stdin()
{
  // -i "filepath"　-pipe 　同時に指定されたらプロセス終了。
  if (NumLoadedFiles != 1) return 1;
  NumLoadedFiles = 0;

  //
  //Input
  //
  _setmode(_fileno(stdin), _O_BINARY);
  fdStdin = _fileno(stdin);

  //
  //StdinHeadFile
  //  標準入力の先頭部をファイルに書き出す、
  //  ファイルにすることでseekに対応する。
  //size
  double gt_10MB = StdinHeadFile_Size_CmdLine;
  gt_10MB = (10 < gt_10MB) ? gt_10MB : 10;                 //greater than 10 MiB
  StdinHeadFile_Size = (int)(gt_10MB * 1024 * 1024);

  //buff
  char *stdinHeadBuff = new char[StdinHeadFile_Size];      //先頭部分保存用のバッファ
  memset(stdinHeadBuff, '\0', StdinHeadFile_Size);

  //
  //Fill stdinHeadBuff
  //  標準入力からデータ取り出し
  //
  int curBuffSize = 0;                       //読込済サイズ

  IsClosed_stdin = true;                     //タイムアウト判定用
  time_t timeReadPipe_begin = time(NULL);    //タイムアウト判定用

  while (curBuffSize < StdinHeadFile_Size)
  {
    int tickDemandSize = StdinHeadFile_Size - curBuffSize;     //要求サイズ
    int readsize = _read(fdStdin, stdinHeadBuff + curBuffSize, tickDemandSize);

    if (readsize == -1)                                        //fail to connect
    {
      //標準入力からデータがくるまで待機、３００秒経過で終了
      //一度接続を確認したらタイムアウトはしない
      if (IsClosed_stdin && time(NULL) - timeReadPipe_begin < 300)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        continue;
      }
      else
        return 1;					                                 //time out.
    }
    else if (readsize == 0)
      return 1;		                                         //end of stream. too small source.

    curBuffSize += readsize;
    IsClosed_stdin = false;
  }

  if (curBuffSize == 0) return 1;		                       //fail to read stdin. exit.

  StdinHeadFile_Size = curBuffSize;


  //
  //windowsのtempフォルダにDGI_pf.tmpを作成し、先頭部をファイルに保存する。
  //  fdで開くと終了時に削除できなかった。FILE*で書き込んで閉じる。
  //

  //書込
  //  file pointer
  FILE *pf_tmp;

  //tmp name
  StdinHeadFile_Path = _tempnam(NULL, "DGI_pf.tmp");
  if (StdinHeadFile_Path == NULL)
    return 1;

  pf_tmp = fopen(StdinHeadFile_Path, "wb");
  if (pf_tmp == NULL)
    return 1;

  //write data
  size_t canwrite = fwrite(stdinHeadBuff, StdinHeadFile_Size, 1, pf_tmp);
  if (canwrite == 0)
    return 1;
  fclose(pf_tmp);

  //読込
  //  file descriptorで再オープン
  fdStdinHeadFile = _open(StdinHeadFile_Path, _O_RDONLY | _O_BINARY);
  if (fdStdinHeadFile == -1)
    return 1;

  strcpy(Infilename[NumLoadedFiles], StdinHeadFile_Path);          //d2vファイル３行目のファイル名
  Infile[NumLoadedFiles] = fdStdinHeadFile;                        //入力ファイルをStdinStreamFileに
  NumLoadedFiles = 1;

  //release buff
  if (stdinHeadBuff != NULL)
  {
    delete[] stdinHeadBuff;
    stdinHeadBuff = NULL;
  }

  return 0;
}





