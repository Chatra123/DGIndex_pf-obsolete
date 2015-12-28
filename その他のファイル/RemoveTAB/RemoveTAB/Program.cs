using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

  /*
   * VisualStudio2013
   * DGIndex　getbit.cpp
   * 
   * getbit.cppでドキュメントのフォーマットをするとマクロ定義部の
   * 水平タブが増殖する。
   * ファイルサイズが肥大化するので、水平タブを削除する。
   * 水平タブの代わりにスペース２個で代用しているので、
   * スペースも削除する。
   * 
   * string  srcname; のように型、変数名の間に２個スペースがあると、
   * つながりコンパイルできなくなるので要確認
   * 
   * #define DEMUX_AC3 \
   * も間のスペースが２個だとつながってコンパイルが通らなくなる。
   * 
   * 
   * ASCII	tab		  0x09
   *        space		0x20
   * 
   */

namespace RemoveTAB
{

  class Program
  {

    static void Main(string[] args)
    {
      string srcname = "getbit.cpp";
      string outname = "getbit.trim_space.cpp";

      if (File.Exists(srcname) == false) return;
      var fs_in = new FileStream(srcname, FileMode.Open, FileAccess.Read, FileShare.Read);
      var reader = new StreamReader(fs_in, Encoding.GetEncoding("Shift_JIS"));
      var fs_out = new FileStream(outname, FileMode.Create, FileAccess.Write, FileShare.Read);
      var writer = new StreamWriter(fs_out, Encoding.GetEncoding("Shift_JIS"));


      while (true)
      {
        if (reader.EndOfStream) break;
        string line = reader.ReadLine();
        line = Regex.Replace(line, @"\t", "", RegexOptions.IgnoreCase);    //タブ
        line = Regex.Replace(line, @"\s\s", "", RegexOptions.IgnoreCase);  //スペース連続
     		writer.WriteLine(line);
      }

      reader.Close();
      writer.Close();
    }
  }
}
