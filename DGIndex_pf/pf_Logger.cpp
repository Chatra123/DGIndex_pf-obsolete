#include "global.h"

//
// pf用のログ
//DGIndexのgui.cppに_fcloseall()があるので毎回オープンする。
//

//
//LogTimeCode更新
//
void Refresh_LogTimeCode()
{
  SYSTEMTIME st;
  GetSystemTime(&st);

  //  st.wHourを９時間足して、日本時間にする
  char szTime[32] = { 0 };

  int hour = st.wHour + 9;
  hour = (hour < 24) ? hour : hour - 24;

  wsprintf(szTime, "%04d/%02d/%02d %02d:%02d:%02d.%03d",
    st.wYear, st.wMonth, st.wDay,
    hour, st.wMinute, st.wSecond, st.wMilliseconds);

  LogTimeCode[0] = '\0';
  sprintf(LogTimeCode, "%s", szTime);
}


//
//TSファイルごとのLog
//
void Logging_ts(char *msg)
{
  if (Enable_pfLog == false) return;

  //ソース名
  char *srcname;
  if (Mode_Stdin)
    srcname = Stdin_SourcePath;
  else
    srcname = Infilename[NumLoadedFiles - 1];

  //close
  if (logger_ts)
    logger_ts.close();

  //reopen
  {
    char  drive[32], directory[256], filename_ts[256], ext[32];
    _splitpath(srcname, drive, directory, filename_ts, ext);

    char logPath[256];
    sprintf(logPath, "%s%s", drive, directory);
    sprintf(logPath, "%s%s%s%s", logPath, filename_ts, ext, ".DGI.log");
    logger_ts = std::ofstream(logPath, std::ios::app);
  }

  //write
  if (logger_ts)
  {
    Refresh_LogTimeCode();
    logger_ts << LogTimeCode << ":  " << msg;
    logger_ts.close();
  }
}


//共有のLog
void Logging_pf(char *msg)
{
  if (Enable_pfLog == false) return;

  //ソース名
  char *srcname;
  if (Mode_Stdin)
    srcname = Stdin_SourcePath;
  else
    srcname = Infilename[NumLoadedFiles - 1];

  //close
  if (logger_pf)
    logger_pf.close();

  //reopen
  {
    char logPath[256] = "E:\\TS Chap\\DGI_error.log";
    logger_pf = std::ofstream(logPath, std::ios::app);
  }

  //write
  if (logger_pf)
  {
    Refresh_LogTimeCode();
    logger_pf << LogTimeCode << std::endl;
    logger_pf << srcname << std::endl;;
    logger_pf << msg;
    logger_pf.close();
  }

}























