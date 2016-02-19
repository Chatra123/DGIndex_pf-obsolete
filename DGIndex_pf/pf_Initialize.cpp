#include "global.h"
//
//デバッグ用  引数
// 
//  -i "E:\TS_Samp\n20s.ts" -o "E:\TS_Samp\n20s.ts" -ia 4 -fo 0 -yr 2 -om 2 -nodialog -limit 10.0
//  -i "E:\TS_Samp\scramble_180s.ts" -o "E:\TS_Samp\scramble_180s.ts" -ia 4 -fo 0 -yr 2 -om 2 -nodialog -limit 10.0
//

//
//パイプ処理の初期化
//parse_cliを処理した直後に実行すること。
//
int Initialize_pf()
{
  //initialize
  IsClosed_stdin = true;
  GetExtraData_fromStdin = false;

  if (Mode_Stdin &&
    Initialize_stdin() == 1)
  {
    //エラー
    remove(StdinHeadFile_Path);
    return 1;                            //プロセス終了
  }

  timeFlushD2VFile = time(NULL);
  tickReadSize_speedlimit = 0;
  tickBeginTime_speedlimit = system_clock::now();
  SpeedLimit = SpeedLimit_CmdLine * 1024 * 1024;    //Byte/sec --> MiB/sec

  //デバッグ用のログ
  {
    Enable_pfLog = false;
#ifdef _DEBUG
    Enable_pfLog = true;//  true  false
    Logger_Initilaize();
#endif
  }

  return 0;
}


//
//Initialize_stdin
//
int Initialize_stdin()
{
  // -i "filepath"　-pipe 　同時に指定されたらプロセス終了。
  if (NumLoadedFiles != 1) return 1;

  //
  //Input
  //
  _setmode(_fileno(stdin), _O_BINARY);
  fdStdin = _fileno(stdin);

  //
  //StdinHeadFile
  //  標準入力の先頭部をファイルに書き出す、
  //  ファイルにすることでseekに対応する。
  //set size
  double filesize = StdinHeadFile_Size_CmdLine;
  filesize = (6 < filesize) ? filesize : 6;                //greater than 6 MiB
  StdinHeadFile_Size = (int)(filesize * 1024 * 1024);

  //buff
  char *stdinHeadBuff = new char[StdinHeadFile_Size];      //先頭部分保存用のバッファ
  memset(stdinHeadBuff, '\0', StdinHeadFile_Size);

  //
  //  標準入力からデータ取り出し
  //
  int curBuffSize = 0;                                     //読込済サイズ

  while (curBuffSize < StdinHeadFile_Size)
  {
    int requestSize = StdinHeadFile_Size - curBuffSize;    //要求サイズ
    int readsize = _read(fdStdin, stdinHeadBuff + curBuffSize, requestSize);

    if (readsize == -1)                                    //fail to connect
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      continue;
    }
    else if (readsize == 0)
      return 1;		                                         //end of stream. too small source.

    curBuffSize += readsize;
  }
  IsClosed_stdin = false;
  if (curBuffSize == 0) return 1;		                       //fail to read stdin. exit.



  //
  //windowsのtempフォルダにDGI_pf_tmp_00000_2を作成し、ストリーム先頭部をファイルに保存する。
  //  fdで書くと終了時に削除できなかった。FILE*で書いて閉じてから、fdで読み込む。
  FILE *tmpfile;

  //tmp file name
  DWORD pid = GetCurrentProcessId();
  int rnd = rand() % (1000 * 1000);
  std::string basename = "DGI_pf_tmp_" + std::to_string(pid) + std::to_string(rnd) + "_";
  StdinHeadFile_Path = _tempnam(NULL, basename.c_str());
  if (StdinHeadFile_Path == NULL)
    return 1;

  tmpfile = fopen(StdinHeadFile_Path, "wb");
  if (tmpfile == NULL)
    return 1;

  //write
  size_t canwrite = fwrite(stdinHeadBuff, StdinHeadFile_Size, 1, tmpfile);
  if (canwrite == 0)
    return 1;
  fclose(tmpfile);

  //release
  if (stdinHeadBuff != NULL)
  {
    delete[] stdinHeadBuff;
    stdinHeadBuff = NULL;
  }

  //  file descriptorで再オープン
  fdStdinHeadFile = _open(StdinHeadFile_Path, _O_RDONLY | _O_BINARY);
  if (fdStdinHeadFile == -1)
    return 1;

  //Infileにセット
  strcpy(Infilename[0], StdinHeadFile_Path);
  Infile[0] = fdStdinHeadFile;

  return 0;
}





