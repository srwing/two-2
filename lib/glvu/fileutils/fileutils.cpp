//------------------------------------------------------------------------------
// File : fileutils.cpp
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

// warning for truncation of template name for browse info
#pragma warning( disable : 4786)


#include <iostream>
#include <vector>
using namespace std;
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fileutils.hpp"

namespace FileUtils {

#ifdef _WIN32
typedef struct _stat stat_t;
inline int dostat(const char *f, stat_t *s) { return _stat(f,s); }
#define S_ISDIR(mode) (mode&S_IFDIR)
#define S_ISREG(mode) (mode&S_IFREG)
#define S_ISLNK(mode) (0)
#else
typedef struct stat stat_t;

inline int dostat(const char *f, stat_t *s) { return ::stat(f,s); }
#endif

//--------------------------------------------------------------------------//
// STAT - QUERY FILE INFO  [internal only for now]
//
// FullPathName - Pointer to the full path name of the file.
// StatStruct - a user-allocated stat struct the call will fill in.
// RETURN - 0 if ok, non-zero error code if error
//--------------------------------------------------------------------------//
static int Stat(const char *FullPathName, stat_t *StatStruct)
{
  // stat() doesn't like trailing dir chars so strip 'em
  string strippedPath(FullPathName);
  StripDirChar(strippedPath);
  int ret = dostat(strippedPath.c_str(), StatStruct);
  return ret; 
}


//--------------------------------------------------------------------------//
// BASENAME - RETURN THE FILE NAME ASSOCIATED WITH A FULL PATH NAME
//
// FullPathName - Pointer to the full path name of the file.
// DirChar - Directory separator character (i.e. '/' or '\\').
// RETURN - STL string. 
//--------------------------------------------------------------------------//
string Basename(const char *FullPathName, char DirChar)
{
  char *p = strrchr(FullPathName, DirChar);

  if (p==NULL) {
    return string(FullPathName);
  } 
  return string(p+1);
}

//--------------------------------------------------------------------------//
// DIRNAME - RETURN THE DIRECTORY NAME ASSOCIATED WITH A FULL PATH NAME
// FullPathName - Pointer to the full path name of the file.
// DirChar - Directory separator character (i.e. '/' or '\\').
// RETURN - STL string.
//--------------------------------------------------------------------------//
string Dirname(const char *FullPathName, char DirChar)
{
  char *p = strrchr(FullPathName, DirChar);

  if (p==NULL) {
    return string();
  }
  return string( FullPathName, p-FullPathName+1 );
}

//--------------------------------------------------------------------------//
// EXTNAME - RETURN THE FILE EXTENSION ASSOCIATED WITH A FULL PATH NAME
// FullPathName - Pointer to the full path name of the file.
// RETURN - STL string.
//--------------------------------------------------------------------------//
string Extname(const char *FullPathName)
{
  char *p = strrchr(FullPathName, '.');

  if (p==NULL) {
    return string();
  }
  return string(p+1);
}

//--------------------------------------------------------------------------//
// STRIPEXT  - RETURN THE FILE WITHOUT ITS ASSOCIATED EXTENSION.
//             WITH A FULL PATH NAME
// FullPathName - Pointer to the full path name of the file.
// RETURN - STL string.
//--------------------------------------------------------------------------//
string StripExt(const char *FullPathName)
{
  const char *p = strrchr(FullPathName, '.');

  if (p==NULL) {
    return string(FullPathName);
  }
  return string(FullPathName, p-FullPathName);
}

//--------------------------------------------------------------------------//
// SPLITPATH  - SPLIT A PATH INTO A VECTOR OF STRINGS.
// FullPathName - Pointer to the full path name of the file.
// splitout - the output of the path split into components
//--------------------------------------------------------------------------//
void SplitPath(const char *FullPathName, vector<string> &splitout, 
                      char dirSep)
{
  
  long f, l;
  string str(FullPathName);
  f = 0;
  // Check for leading separator
  if ( str.find(dirSep) == 0 ) {
    f++;
  }
  while ( (l = str.find(dirSep, f)) != string::npos )
  {
    string s( str.begin()+f, str.begin()+l );
    splitout.push_back( s );
    f = l+1;
  }
  if (f!= str.end() - str.begin()) 
  {
    string s( str.begin()+f, str.end() );
    splitout.push_back( s ); 
  }

}

//--------------------------------------------------------------------------//
// JOINPATH  - Join a vector of strings together into a path separated by
//             the dirchar
// split - Vector of string path parts
// dirSet - The character for separating directory components
//--------------------------------------------------------------------------//
string JoinPath(const vector<string> &splitPath, char dirSep)
{
  string str;
  vector<string>::const_iterator i = splitPath.begin();
  for (; i!=splitPath.end(); ++i)
  {
    str += *i;
    if (i!=splitPath.end()-1)
      str += dirSep;
  }
  return str;
}

//--------------------------------------------------------------------------//
// CHDIR - Change the current directory to DirName.
// DirName - Pointer to the directory name to change.
// RETURN - 0 if failure, 1 if success.
//--------------------------------------------------------------------------//
int ChDir(const char *DirName)
{
  int result = 
#ifdef _WIN32
  _chdir(DirName);
#else
  chdir(DirName);
#endif
  return (result==0)?1:0;
}

//--------------------------------------------------------------------------//
// GETDIR - GET THE CURRENT DIRECTORY NAME.
// RETURN - STL string.
//--------------------------------------------------------------------------//
string GetDir()
{
  // _GETCWD MALLOCS A BUFFER. REMEMBER TO FREE IT.
#ifdef _WIN32
  char *result = _getcwd(NULL,0);
#elif defined(__linux__)
  char *result =  getcwd(NULL,0);
#else
  char *result =  getcwd(NULL,(size_t)-1);
#endif
  if (result != NULL) {
    string strresult(result);
    // FREE MALLOC'ED BUFFER.
    free(result);
    return strresult;
  }
  return string();
}

//--------------------------------------------------------------------------//
// MKDIR  - CREATE A NEW DIRECTORY.
// RETURN - 0 on failure, 1 on success
//--------------------------------------------------------------------------//
int MkDir(const char *DirName)
{
  int result = 
#ifdef _WIN32
  _mkdir(DirName);
#else
  // set mode of directory to drwxr-xr-x.
  mkdir(DirName, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif
  return (result==0)?1:0;
}

//--------------------------------------------------------------------------//
// ISDIR  - CHECK IF THE GIVEN STRING IS ACTUALLY A DIRECTORY.
// RETURN - 0 if failure, 1 if success.
//--------------------------------------------------------------------------//
int IsDir(const char *DirName)
{
  stat_t st;
  if (Stat(DirName, &st)) return 0;
  return S_ISDIR(st.st_mode) ? 1 : 0;
}

//--------------------------------------------------------------------------//
// ISFILE  - CHECK IF THE GIVEN STRING IS A PLAIN FILE.
// RETURN - 0 if failure, 1 if success.
//--------------------------------------------------------------------------//
int IsFile(const char *FileName)
{
  stat_t st;
  if (Stat(FileName, &st)) return 0;
  return S_ISREG(st.st_mode) ? 1 : 0;
}

//--------------------------------------------------------------------------//
// ISLINK  - CHECK IF THE GIVEN STRING IS A SYMBOLIC LINK.
// RETURN - 0 if failure, 1 if success.
//--------------------------------------------------------------------------//
int IsLink(const char *FileName)
{
#ifndef _WIN32
  stat_t st;
  if (lstat(FileName, &st)) return 0;
  return S_ISLNK(st.st_mode) ? 1 : 0;
#else
  return 0;
#endif
}


//--------------------------------------------------------------------------//
// FILESTATUS  - PERFORM MULTIPLE CHECKS ON FILE ATTRIBUTES.
// RETURN - bit mask of some combination of the flags:
//           IS_DIR, IS_EXE, IS_READABLE, IS_WRITEABLE, 
//           IS_FILE, IS_LINK, FILE_EXISTS 
//   (if you need to check multiple attributes this is more efficient
//    than individual calls to IsDir() IsReadable() etc
//--------------------------------------------------------------------------//
unsigned int FileStatus( const char *FileName )
{
  stat_t st;
  if (Stat(FileName, &st)) return 0;
  unsigned int result = FILE_EXISTS;
#ifndef _WIN32
  stat_t lst;
  lstat(FileName, &lst);
  if S_ISLNK(lst.st_mode)  result |= IS_LINK;
#endif
  if S_ISDIR(st.st_mode)     result |= IS_DIR;
  if S_ISREG(st.st_mode)     result |= IS_FILE;
  if (st.st_mode & S_IEXEC)  result |= IS_EXE;
  if (st.st_mode & S_IREAD)  result |= IS_READABLE;
  if (st.st_mode & S_IWRITE)  result |= IS_WRITEABLE;
  return result;
}

//--------------------------------------------------------------------------//
// ISREADABLE  - CHECK IF THE GIVEN STRING IS A READABLE FILE.
// RETURN - 0 if failure, 1 if success.
//--------------------------------------------------------------------------//
int IsReadable(const char *PathName)
{
  stat_t st;
  if (Stat(PathName, &st)) return 0;
  return (st.st_mode & S_IREAD) ? 1 : 0;
}

//--------------------------------------------------------------------------//
// ISWRITEABLE  - CHECK IF THE GIVEN STRING IS WRITEABLE.
// RETURN - 0 if failure, 1 if success.
//--------------------------------------------------------------------------//
int IsWriteable(const char *PathName)
{
  stat_t st;
  if (Stat(PathName, &st)) return 0;
  return (st.st_mode & S_IWRITE) ? 1 : 0;
}

//--------------------------------------------------------------------------//
// ISEXE  - CHECK IF THE GIVEN STRING IS EXECUTABLE (OR SEARCHABLE FOR A DIR).
// RETURN - 0 if failure, 1 if success.
//--------------------------------------------------------------------------//
int IsExe(const char *PathName)
{
  stat_t st;
  if (Stat(PathName, &st)) return 0;
  return (st.st_mode & S_IEXEC) ? 1 : 0;
}


//--------------------------------------------------------------------------//
// FILEEXISTS  - SEE IF A FILE EXISTS.
// RETURN - 0 if failure, 1 if success.
//--------------------------------------------------------------------------//
int FileExists(const char *FileName)
{
  stat_t st;
  if (!Stat(FileName, &st)) {
    return 1;
  }
  return 0;
}

//--------------------------------------------------------------------------//
// ConvertDirChar - Change dir chars in path char to given char
//--------------------------------------------------------------------------//
string &ConvertDirChar(string &path, char dchar)
{
  string::size_type start = 0;
  for (;;) {
    start = path.find_first_of("\\/", start);
    if (start == string::npos) break;
    path[start] = dchar;
    start++;
  }
  return path;
}

//--------------------------------------------------------------------------//
// ConvertDirChar - Change dir chars in path char to given char
//--------------------------------------------------------------------------//
string ConvertDirChar(const char *path, char dchar)
{
  string strpath(path);
  return ConvertDirChar(strpath);
}

//--------------------------------------------------------------------------//
// StripDirChar - Remove the terminal dirchars from a path, if any
//--------------------------------------------------------------------------//
string &StripDirChar(string &path)
{
  for(;;) {
    int found = path.find_first_of("\\/", path.size()-1);
    if (found == string::npos) break;
    path.erase(found);
  }
  return path;
}

//--------------------------------------------------------------------------//
// ReadTextFile - Open the specified file as text and return its contents
// as a string.
//--------------------------------------------------------------------------//
string ReadTextFile(const char *filename)
{
  string contents;
  stat_t stats;

  if (!filename) return contents;

  if (Stat(filename, &stats) != 0) 
  {
    fprintf(stderr,"Cannot open \"%s\" for stat read!\n", filename);
    return contents;
  }

  long size = stats.st_size;
  char *buffer = new char[size + 1];

  FILE *fp = 0;
  if (!(fp = fopen(filename, "r")))
	{
		fprintf(stderr,"Cannot open \"%s\" for read!\n", filename);
		return contents;
	}

	int bytes;
	bytes = fread(buffer, 1, size, fp);

  buffer[bytes] = 0;

	fclose(fp);
  contents += buffer;
  return contents;
}


//-----------------------------------------------------------------------------
// @ FileFinder::NewSearch()
// ---------------------------------------------------------------------------
// Instantiate a new search. If FindDirs is true, also returns directories,
// which are guaranteed to end with '/'.
//-----------------------------------------------------------------------------
void FileFinder::NewSearch(const char *searchStr, bool findDirs)
{
  FFindDirs = findDirs;
  if (SearchActive()) {
    SearchClose();
  }
#ifdef _WIN32
  FWildCard = searchStr;
  string FTPath = Dirname(FWildCard.c_str());
  if (FTPath.empty()) { 
    FPath = "";
  } else {
    FPath = FTPath;
  }
  FCallFindFirst = true;
#else
  if (glob(searchStr, GLOB_MARK, NULL, &FFindInfo) == 0) {
    FFindPos = 0;
  }
#endif
}

//-----------------------------------------------------------------------------
// @ FileFinder::GetFromFindInfo()
// ---------------------------------------------------------------------------
// For internal use of WIN32 version.
// Have to manually prepend the path to the file name returned.
// Also have to make sure we don't return ./ and ../
//-----------------------------------------------------------------------------
#ifdef _WIN32
string FileFinder::GetFromFindInfo()
{
  if (!(FFindInfo.attrib & _A_SUBDIR)) {
    string p(FPath);
    p += FFindInfo.name;
    return p;
  } else if (FFindDirs && 
             (strcmp(FFindInfo.name,".") != 0) && 
             (strcmp(FFindInfo.name,"..") != 0) ) 
  {
    string p(FPath);
    p += FFindInfo.name;
    AddDirChar(p);
    return p;
  } else {
    return Search();
  }
}
#endif

//-----------------------------------------------------------------------------
// @ FileFinder::Search()
// ---------------------------------------------------------------------------
// Continue search. If no more files are left, will return the empty string.
//-----------------------------------------------------------------------------
string FileFinder::Search()
{
#ifdef _WIN32
  if (FCallFindFirst) {
    FCallFindFirst = false;
    FFindHandle = _findfirst(FWildCard.c_str(), &FFindInfo);
    if (FFindHandle != -1l) {
      return GetFromFindInfo();
    }
  } else {
    if (_findnext(FFindHandle, &FFindInfo) != -1) {
      return GetFromFindInfo();
    }
  }
  return string();
#else
  while (FFindPos < FFindInfo.gl_pathc) {
    char *currFile = FFindInfo.gl_pathv[FFindPos];
    int len = strlen(currFile);
    FFindPos++;
    if (FFindDirs || currFile[len-1] != DefaultDirChar) {
      return string(currFile);
    }
  }
  return string();
#endif
}

//-----------------------------------------------------------------------------
// @ FileFinder::DeepSearch()
// ---------------------------------------------------------------------------
// Do a recursive search.
// Not very efficient. two while loops to go through files then dirs.
//-----------------------------------------------------------------------------
bool FileFinder::DeepSearch(const char *searchPath, const char *wildcard, bool findDirs, DeepSearchFunc func)
{
  string searchStr(searchPath);
  AddDirChar(searchStr);
  searchStr += wildcard;

  bool result = DeepSearch(searchStr.c_str(), findDirs, func);

  return result;
}

//-----------------------------------------------------------------------------
// @ FileFinder::DeepSearch()
// ---------------------------------------------------------------------------
// Do a recursive search.
// Not very efficient. two while loops to go through files then dirs.
//-----------------------------------------------------------------------------
bool FileFinder::DeepSearch(const char *searchStr, bool findDirs, DeepSearchFunc func)
{
  string s;
  
  NewSearch(searchStr,false); // do a search for matching FILES only
  while (!(s = Search()).empty()) {
    if (!func(s.c_str())) {
      return false;
    }
  }

  string rootPath = Dirname(searchStr);
  string wildCard = Basename(searchStr);

  // now set up the recursive search. Find ALL directories ("*" search), 
  // then call DeepSearch with the given wildcard on the subdirs.
  int len = rootPath.size();
  string str(rootPath);
  str += '*';
  NewSearch(str.c_str(),true);

  bool ok = true;
  while (ok && !(s = Search()).empty()) {
    if (s[s.size()-1] == DefaultDirChar) {  // is a dir
      if (findDirs && !func(s.c_str())) {
        ok = false;
        continue;
      }
      FileFinder deeperFinder;
      string newSearchStr(s);
      newSearchStr += wildCard;
      ok = deeperFinder.DeepSearch(newSearchStr.c_str(),findDirs,func);
    }
  }
  return ok;
}

//-----------------------------------------------------------------------------
// @ FileFinder::DeepSearch()
// ---------------------------------------------------------------------------
// Do a recursive search. Like previous DeepSearch, but callback gives you a list of
// all files matching the wildcard in the current directory.
// Not very efficient. two while loops to go through files then dirs.
//-----------------------------------------------------------------------------
bool FileFinder::DeepSearch(const char *searchPath, const char *wildcard, DeepSearchExtFunc func)
{
  string searchStr(searchPath);
  AddDirChar(searchStr);
  searchStr += wildcard;
  
  bool result = DeepSearch(searchStr.c_str(), func);

  return result;
}

//-----------------------------------------------------------------------------
// @ FileFinder::DeepSearch()
// ---------------------------------------------------------------------------
// Do a recursive search. Like previous DeepSearch, but callback gives you a list of
// all files matching the wildcard in the current directory.
// Not very efficient. two while loops to go through files then dirs.
//-----------------------------------------------------------------------------
bool FileFinder::DeepSearch(const char *searchStr, DeepSearchExtFunc func)
{
  string s; 
  std::vector<string> files;
  
  NewSearch(searchStr,false);
  while (!(s = Search()).empty()) {
    files.push_back(s);
  }
  func(files);
  files.resize(0);

  string rootPath = Dirname(searchStr);
  string wildCard = Basename(searchStr);

  int len = rootPath.size();
  string str(rootPath);
  str += '*';
  NewSearch(str.c_str(),true);
  bool ok = true;
  while (ok && !(s = Search()).empty()) {
    if (s[s.size()-1] == DefaultDirChar) {  // is a dir
      FileFinder deeperFinder;
      string newSearchStr(s);
      newSearchStr += wildCard;
      ok = deeperFinder.DeepSearch(newSearchStr.c_str(),func);
    }
  }
  return ok;
}

//-----------------------------------------------------------------------------
// @ FileFinder::DeepSearch()
// ---------------------------------------------------------------------------
// Do a recursive search.
//-----------------------------------------------------------------------------
bool FileFinder::DeepSearch(const char *searchPath, const char *wildcard, bool findDirs, DeepSearchExt2Func func, void *userData)
{
  string searchStr(searchPath); 
  AddDirChar(searchStr);
  searchStr += wildcard;
  
  bool result = DeepSearch(searchStr.c_str(), findDirs, func, userData);

  return result;
}

//-----------------------------------------------------------------------------
// @ FileFinder::DeepSearch()
// ---------------------------------------------------------------------------
// Do a recursive search.
//-----------------------------------------------------------------------------
bool FileFinder::DeepSearch(const char *searchStr, bool findDirs, DeepSearchExt2Func func, void *userData)
{
  string s;
  
  NewSearch(searchStr,false); // do a search for matching FILES only
  while (!(s = Search()).empty()) {
    if (!func(s.c_str(), userData)) {
      return false;
    }
  }

  string rootPath = Dirname(searchStr);
  string wildCard = Basename(searchStr);

  // now set up the recursive search. Find ALL directories ("*" search), 
  // then call DeepSearch with the given wildcard on the subdirs.
  int len = rootPath.size();
  string str(rootPath);
  str += '*';
  NewSearch(str.c_str(),true);
  bool ok = true;
  while (ok && !(s = Search()).empty() ) {
    if (s[s.size()-1] == DefaultDirChar) {  // is a dir
      if (findDirs && !func(s.c_str(),userData)) {
        ok = false;
        continue;
      }
      FileFinder deeperFinder;
      string newSearchStr(s);
      newSearchStr += wildCard;
      ok = deeperFinder.DeepSearch(newSearchStr.c_str(),findDirs,func, userData);
    }
  }
  return ok;
}

//-----------------------------------------------------------------------------
// @ FileFinder::SearchActive()
// ---------------------------------------------------------------------------
// Are we currently searching?
//-----------------------------------------------------------------------------
bool FileFinder::SearchActive()
{
#ifdef _WIN32
  return FFindHandle != -1;
#else
  return FFindPos >= 0;
#endif
}

//-----------------------------------------------------------------------------
// @ FileFinder::SearchClose()
// ---------------------------------------------------------------------------
// Reset any state altered by performing the search.
// which are guaranteed to end with '/'.
//-----------------------------------------------------------------------------
void FileFinder::SearchClose()
{
  if (SearchActive()) {
  #ifdef _WIN32
    _findclose(FFindHandle);
    FCallFindFirst = true;
    FFindHandle = -1;
  #else
    globfree(&FFindInfo);
    FFindPos = -1;
  #endif
  }
}

//-----------------------------------------------------------------------------
// @ FileFilter::FilterFiles()
// ---------------------------------------------------------------------------
// API entry point. See header file for explanation.
//-----------------------------------------------------------------------------
void FileFilter::FilterFiles(char *srcDir, char *wildcard, char *destDir, bool recurse, FilterFunc func)
{
  FileFinder finder;

  string FSrcDir(srcDir);
  AddDirChar(FSrcDir);

  FWildcard = wildcard;
  FDestDir  = destDir;
  FFilterFunc = func;

  string oldDir = GetDir();

  // Make destination directory if it doesn't already exist.
  if (!ChDir(FDestDir.c_str())) {
    if (!MkDir(FDestDir.c_str())) {
      std::cerr << "Could not make directory " << FDestDir << ". Aborting..." << std::endl;
      return;
    }
  } else {
    ChDir(oldDir.c_str());
  }
 
  if (!FSrcDir.empty() && !ChDir(FSrcDir.c_str())) {
    cerr << "Could not ChDir() to " << FSrcDir << endl;
    return;
  } else {
    ChDir(oldDir.c_str());
  }

  if (recurse) {
    finder.DeepSearch(FSrcDir.c_str(), FWildcard.c_str() ,true,DoFilter,this);
  } else {
    string s;
    finder.NewSearch(FWildcard.c_str());
    while (!(s = finder.Search()).empty() && DoFilter(s.c_str(),this)) {

    }
  }

}

//-----------------------------------------------------------------------------
// @ FileFilter::DoFilter()
// ---------------------------------------------------------------------------
// Internal use.
//-----------------------------------------------------------------------------
bool FileFilter::DoFilter(const char *filename, void *userData)
{
  FileFilter *filter = (FileFilter *)userData;

  assert(strstr(filename, filter->FSrcDir.c_str()) == filename);
  string dname = Dirname(filename+strlen(filter->FSrcDir.c_str()));

  // figure out the new directory name.
  string newDir(filter->FDestDir);
  AddDirChar(newDir);
  if (!dname.empty()) newDir+=dname;

  
  if (filename[strlen(filename)-1] == DefaultDirChar) {    // if it's just a dir, then just create the new dir.
    string old = GetDir();
    if (!ChDir(newDir.c_str())) {
      if (!MkDir(newDir.c_str())) {
        cerr << "Could not make directory " << newDir << ". Aborting..." << endl;
        return false;
      }
    } else {
      ChDir(old.c_str());
    }
  } else {   // otherwise, call the FilterFunc
    filter->FFilterFunc(filename, newDir.c_str());
  }

  return true;
}


}
