using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

namespace RemoveTAB
{
	/*
	 * VisualStudio2013
	 * DGIndex　getbit.cpp
	 * 
	 * getbit.cppでドキュメントのフォーマットをするとマクロ定義部の水平タブが増殖する。
	 * ファイルサイズが肥大化するので、水平タブのみを削除する。
	 */
	class Program
	{
		/* ASCII	tab		0x09 */
		static void Main(string[] args)
		{
			string srcname = "getbit.cpp";
			string outname = "getbit.out.cpp";

			if (File.Exists(srcname) == false) return;
			var fs_in = new FileStream(srcname, FileMode.Open, FileAccess.Read, FileShare.Read);
			var reader = new StreamReader(fs_in, Encoding.GetEncoding("Shift_JIS"));
			var fs_out = new FileStream(outname, FileMode.Create, FileAccess.Write, FileShare.Read);
			var writer = new StreamWriter(fs_out, Encoding.GetEncoding("Shift_JIS"));


			while (true)
			{
				if (reader.EndOfStream) break;
				string line = reader.ReadLine();
				line = Regex.Replace(line, "\t", "", RegexOptions.IgnoreCase);//タブ削除
				writer.WriteLine(line);
			}

			reader.Close();
			writer.Close();
			Console.WriteLine("finish");
			Console.WriteLine("press key to exit");
			Console.Read();
		}
	}
}
