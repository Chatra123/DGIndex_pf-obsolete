#include "global.h"

using namespace std;
using namespace std::chrono;


//
//初期化　ロガー用設定
//
void Logger_Initilaize()
{
  if (Enable_pfLog == false) return;

  //ソース名
  char* srcname = NULL;
  {
    if (Mode_Stdin)
      srcname = Stdin_SourcePath;
    else if (0 < NumLoadedFiles)
      srcname = Infilename[NumLoadedFiles - 1];
  }

  //logpath_ts作成
  if (srcname != NULL)
  {
    log_srcname = srcname;

    char  drive[32], directory[256], filename_ts[256], ext[32];
    _splitpath(srcname, drive, directory, filename_ts, ext);

    string str_drv = drive;
    string str_dir = directory;
    string str_name = filename_ts;
    string str_ext = ext;
    logpath_ts = str_drv + str_dir + str_name + str_ext + ".DGI.log";
  }
}



//
//TimeCode取得
//
string Get_TimeCode()
{
  SYSTEMTIME st;
  GetSystemTime(&st);
  char szTime[32] = { 0 };

  //  st.wHourを９時間足して、日本時間にする
  int hour, day;
  hour = st.wHour + 9;
  day = (24 <= hour) ? st.wDay + 1 : st.wDay;
  hour = (hour < 24) ? hour : hour - 24;

  wsprintf(szTime, "%04d/%02d/%02d %02d:%02d:%02d.%03d",
    st.wYear, st.wMonth, day,
    hour, st.wMinute, st.wSecond, st.wMilliseconds);

  string timecode = szTime;
  return timecode;
}


//
//DGIndex同士で共有Logファイル
//　排他制御はしない
//　gui.cppに_fcloseall()があるのでLogは毎回オープンする。
//
void Logging_pf(char *msg)
{
  if (Enable_pfLog == false) return;

  //reopen
  string logpath_pf = "E:\\TS Chap\\DGI_error.log";
  ofstream log;
  log = std::ofstream(logpath_pf, std::ios::app);

  if (log)
  {
    log << Get_TimeCode() << std::endl;
    log << log_srcname << std::endl;
    log << msg;
    log.close();
  }
}


//
//TSファイルごとのLog
//
void Logging_ts(char *msg)
{
  string str_msg = msg;
  Logging_ts(str_msg);
}

void Logging_ts(string msg)
{
  if (Enable_pfLog == false) return;

  //reopen
  std::ofstream log;
  log = std::ofstream(logpath_ts, std::ios::app);

  if (log)
  {
    log << Get_TimeCode() << " :   " << msg.c_str();
    log.close();
  }
}





