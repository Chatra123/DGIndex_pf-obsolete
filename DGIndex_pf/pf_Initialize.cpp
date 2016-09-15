/*
メモ
◇　skl_nasm.hに対して”ドキュメントのフォーマット”を実行した後にリビルドすると
　　失敗する。skl_nasm.hに”ドキュメントのフォーマット”を実行してはいけない。
　　　　error MSB6006: "cmd.exe" はコード 1 を伴って終了しました。


◇　getbit.cppに”ドキュメントのフォーマット”を実行するとマクロ定義部の水平タブが増殖し、
　　ファイルサイズがわずかに増える。


*/

#include "global.h"


//パイプ入力の初期化
//parse_cli()を処理した直後に実行すること。
int Initialize_pf()
{
  if (Mode_PipeInput &&
    Initialize_stdin() == 1)
  {
    if (fdHeadFile != -1)
      _close(fdHeadFile);
    remove(HeadFilePath.c_str());
    return 1;                            //プロセス終了
  }

  IsClosed_stdin = false;
  GetExtraData_fromStdin = false;
  timeFlushD2VFile = time(NULL);
  tickReadSize_speedlimit = 0;
  tickBeginTime_speedlimit = system_clock::now();
  SpeedLimit = SpeedLimit_CmdLine * 1024 * 1024;
  return 0;
}



//HeadFile作成
int Initialize_stdin()
{
  // -i "filepath" -pipe が同時に指定されていた。
  if (NumLoadedFiles != 1)
    return 1;

  //HeadFile
  //  標準入力の先頭部をファイルに書き出す。
  //  ファイルにすることでseekに対応
  _setmode(_fileno(stdin), _O_BINARY);
  fdStdin = _fileno(stdin);

  //WindowsのTempフォルダにDGI_temp_0000000_2を作成
  {
    DWORD pid = GetCurrentProcessId();
    std::random_device rnd;
    std::mt19937 mt(rnd());
    UINT32 rnd_num = mt() % (1000 * 1000);
    std::string tmp_path;
    tmp_path = "DGI_temp_" + std::to_string(pid) + std::to_string(rnd_num) + "_";
    tmp_path = std::string(_tempnam(NULL, tmp_path.c_str()));
    HeadFilePath = tmp_path;
  }
  fdHeadFile = _open(HeadFilePath.c_str(), _O_CREAT | _O_RDWR | _O_BINARY, _S_IREAD | _S_IWRITE);
  if (fdHeadFile == -1)
    return 1;
  int head_size;
  {
    const double mini_MB = 6.0;
    double size = HeadFileSize_CmdLine;
    size = mini_MB < size ? size : mini_MB;
    head_size = (int)(size * 1024 * 1024);
  }


  //標準入力からデータ取得、HeadFile書込
  const int buff_size = 1024 * 32;
  std::vector<BYTE> buff(buff_size);
  int cur_size = 0;                  //HeadFileに書込済サイズ

  while (cur_size < head_size)
  {
    int read = _read(fdStdin, &buff.front(), buff_size);
    if (read == -1)
    {
      Sleep(30);
      continue;//fail to connect. wait.
    }
    else if (read == 0)
      break;//end of stream.

    int written = _write(fdHeadFile, &buff.front(), read);
    if (written != read)
      return 1;
    cur_size += written;
  }
  if (head_size <= cur_size)
    head_size = cur_size;
  else
    return 1;//fail to read. too short stream.


  //Infileにセット
  strcpy(Infilename[0], HeadFilePath.c_str());
  Infile[0] = fdHeadFile;
  _lseeki64(Infile[0], 0, SEEK_SET);

  return 0;
}



