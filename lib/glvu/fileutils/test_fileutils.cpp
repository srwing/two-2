//------------------------------------------------------------------------------
// File : test_fileutils.cpp
//------------------------------------------------------------------------------
// GLVU : Copyright 1997 - 2002 
//        The University of North Carolina at Chapel Hill
//------------------------------------------------------------------------------
// Permission to use, copy, modify, distribute and sell this software and its 
// documentation for any purpose is hereby granted without fee, provided that 
// the above copyright notice appear in all copies and that both that copyright 
// notice and this permission notice appear in supporting documentation. 
// Binaries may be compiled with this software without any royalties or 
// restrictions. 
//
// The University of North Carolina at Chapel Hill makes no representations 
// about the suitability of this software for any purpose. It is provided 
// "as is" without express or implied warranty.

#include "fileutils.hpp"

#include <iostream>
#include <string>
using namespace std;
using namespace FileUtils;

void doASearch(const char *pattern, bool withDirs = false)
{
  FileFinder ff;
  ff.NewSearch(pattern,withDirs);
  string file;
  int num =0;
  while (!(file = ff.Search()).empty())
  {
    // Test out both ways of getting the information
    if (num++%2) {
      // The slow way to get all the bits:
      cout 
        << (FileExists(file.c_str())?"e":"-")
        << (IsFile(file.c_str())?"p":"-")
        << (IsDir(file.c_str())?"d":"-")
        << (IsReadable(file.c_str())?"r":"-")
        << (IsWriteable(file.c_str())?"w":"-")
        << (IsExe(file.c_str())?"x":"-")
        << (IsLink(file.c_str())?"l":"-")
        << "\t" << file.c_str() << endl;
    }
    else {
      // The efficient way to get them all:
      unsigned int st = FileStatus( file.c_str() );
      cout 
        << ((st&FILE_EXISTS) ?"e":"-")
        << ((st&IS_FILE)     ?"p":"-")
        << ((st&IS_DIR)      ?"d":"-")
        << ((st&IS_READABLE) ?"r":"-")
        << ((st&IS_WRITEABLE)?"w":"-")
        << ((st&IS_EXE)      ?"x":"-")
        << ((st&IS_LINK)     ?"l":"-")
        << "\t" << file.c_str() << endl;
    }
  }
}

bool deepSearchCallback(const char *file)
{
  cout << file << endl;
  return true;
}
void doADeepSearch(const char *pattern, bool withDirs=false)
{
  FileFinder ff;
  ff.DeepSearch(pattern, withDirs, deepSearchCallback);
}

void misc_api_tests()
{
  cout << "Non-existent file stat test:" 
       << (FileExists("_foobieqzzsdfse.sfa") ? "failed" : "passed") << endl;

  {
    string s("foobie/doodle//");
    cout << "StripDirChar(\"" << s.c_str() << "\") => " << StripDirChar(s).c_str() << endl;
    s += "/";
    cout << "StripDirChar(\"" << s.c_str() << "\") => " << StripDirChar(s).c_str() << endl;
    cout << "StripDirChar(\"" << s.c_str() << "\") => " << StripDirChar(s).c_str() << endl;
  }
  {
    string s("foobie\\doodle\\\\");
    cout << "StripDirChar(\"" << s.c_str() << "\") => " << StripDirChar(s).c_str() << endl;
    s += "\\";
    cout << "StripDirChar(\"" << s.c_str() << "\") => " << StripDirChar(s).c_str() << endl;
    cout << "StripDirChar(\"" << s.c_str() << "\") => " << StripDirChar(s).c_str() << endl;
  }
  {
    const char *cs = "foobie\\doodle\\\\";
    string s(cs);
    cout << "ConvertDirChar(\"" << s.c_str() << "\") => " << ConvertDirChar(s).c_str() << endl;
    s = cs;
    cout << "ConvertDirChar(\"" << s.c_str() << "\", '/') => " << ConvertDirChar(s,'/').c_str() << endl;
    s = cs;
    cout << "ConvertDirChar(\"" << s.c_str() << "\", '\\') => " << ConvertDirChar(s,'\\').c_str() << endl;
  }
  {
    const char *cs = "foobie/doodle//";
    string s(cs);
    cout << "ConvertDirChar(\"" << s.c_str() << "\") => " << ConvertDirChar(s).c_str() << endl;
    s = cs;
    cout << "ConvertDirChar(\"" << s.c_str() << "\", '/') => " << ConvertDirChar(s,'/').c_str() << endl;
    s = cs;
    cout << "ConvertDirChar(\"" << s.c_str() << "\", '\\') => " << ConvertDirChar(s,'\\').c_str() << endl;
  }

  {
    string cs = "foobie/doodle/zip.splat.poink";
    ConvertDirChar(cs);
    string s(cs);
    cout << "StripExt(\"" << s.c_str() << "\") => " << StripExt(s.c_str()).c_str() << endl;
    s = cs;
    cout << "Extname(\"" << s.c_str() << "\") => " << Extname(s.c_str()).c_str() << endl;
    s = cs;
    cout << "Dirname(\"" << s.c_str() << "\") => " << Dirname(s.c_str()).c_str() << endl;
    s = cs;
    cout << "Basename(\"" << s.c_str() << "\") => " << Basename(s.c_str()).c_str() << endl;
  }

  // Join and split
  {
    const char *strs[] = {
      "foobie/doodle/zip.splat.poink",
      "/foobie/doodle/zip.splat.poink",
      "/foobie/doodle/",
      0
    };
    const char **pstrs = strs;
    while (*pstrs) {
      string cs = ConvertDirChar(*pstrs);
      string s(cs);
      vector<string> parts;
      cout << "SplitPath(\"" << s.c_str() << "\") => \n";
      SplitPath(s.c_str(), parts);
      vector<string>::iterator i = parts.begin();
      for (;i!=parts.end(); ++i)
        cout << "\t" << i->c_str() <<endl;

      cout << "JoinPath() => " << JoinPath(parts).c_str() << endl;
      pstrs++;
    }
  }
}


int main(int argc, char *argv[])
{
  string cwd(GetDir());
  cout << "current dir is: " << cwd.c_str() << endl;
  ChDir("..");
  cout << "new dir is: " << GetDir().c_str() << endl;
  cout << "------\nDirectory file contents:\n";
  doASearch("*");
  cout << "------\nDirectory file+dir contents:\n";
  doASearch("*", true);
  cout << "------\nDeep search for make files:\n";
  doADeepSearch("*make*");
  cout << "------\nGoing back to original dir" <<endl;
  ChDir(cwd.c_str());
  cout << "new dir is: " << GetDir().c_str() << endl;

  cout << "------" << endl;
  misc_api_tests();

  return 0;
}
