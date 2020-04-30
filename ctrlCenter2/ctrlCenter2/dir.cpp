#include "dir.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define Msize (1024.00*1024.00)

bool operator < (const one_file & a, const one_file & b){
	return a.last_write > b.last_write;
}

#ifdef WIN32

#include <windows.h>
#include <direct.h>
#include <io.h>

void FileTime2time_t(FILETIME ft, unsigned *t)
{  
	LONGLONG ll;

	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;

	ll = ((unsigned __int64)ft.dwHighDateTime << 32) + ft.dwLowDateTime;

	*t = (DWORD)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

void list_dir(const string& dir, priority_queue<one_file>& v, unsigned long long & total_size)
{
	WIN32_FIND_DATAA FindFileData;
	DWORD dwError = 0;

	HANDLE hFind = FindFirstFileA((dir + "\\*").c_str(), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
	}
	else 
	{
		do
		{
			string name = FindFileData.cFileName;
			if(name == ".." || name == ".")continue;

			one_file of;
			of.name = dir + "\\";
			of.name += name;
			FileTime2time_t(FindFileData.ftLastWriteTime, (unsigned int *)&of.last_write);
			of.size = FindFileData.nFileSizeLow;
			of.dir = FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			v.push(of);

			total_size += of.size;

			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string sd = dir;
				sd += "\\";
				sd += FindFileData.cFileName;
				list_dir(sd, v, total_size);
			}
		}
		while(FindNextFileA(hFind, &FindFileData) != 0);

		dwError = GetLastError();
		FindClose(hFind);
	}
}

bool is_dir_empty(string& dir)
{

	WIN32_FIND_DATAA FindFileData;

	HANDLE hFind = FindFirstFileA((dir + "\\*").c_str(), &FindFileData);

	bool b = true;
	if (hFind == INVALID_HANDLE_VALUE) 
	{
	}
	else 
	{
		while(FindNextFileA(hFind, &FindFileData) != 0) 
		{
			string name = FindFileData.cFileName;
			if(name == ".." || name == ".")continue;

			b = false;
			break;
		}

		FindClose(hFind);
	}

	return b;
}

void try_clean(priority_queue<one_file>& v, unsigned& deled, unsigned will_del)
{
	while(!v.empty() && deled < will_del){
		one_file of = v.top();
		v.pop();
		if(!of.dir){
			if(0 == _unlink(of.name.c_str())){
				deled += of.size;
			}
			else{
			}
		}
		else if(is_dir_empty(of.name)){
			 _rmdir(of.name.c_str());
		}
	}
}

double dir_freespace(const char * dir)
{
	ULARGE_INTEGER av;
	if(!GetDiskFreeSpaceExA(dir, &av, 0, 0))
	{
		printf("GetDiskFreeSpaceExA fail\n");
		return 0;
	}

	double free = av.QuadPart / Msize;
	return free;
}

void hide_system(char * cmd)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = SW_HIDE;

    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process. 
    if( !CreateProcessA( NULL,   // No module name (use command line)
        cmd,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NO_WINDOW,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf( "CreateProcess failed (%d)\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}


void create_dir(const char * dir)
{
	char buffer[1024];
	sprintf(buffer, "cmd.exe /c mkdir %s", dir);
	hide_system(buffer);
}

std::string current_dir(){
	char path[_MAX_PATH];
	GetModuleFileNameA(0, path, _MAX_PATH);
	char * zero = strrchr(path, '\\');
	*(++zero) = 0;
	return path;
}

#else

#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
//#include "local_addr.h"
#include <unistd.h>

#define _access access

void list_dir(const string& dir, priority_queue<one_file>& v, unsigned long long & total_size)
{
	DIR *dp = opendir(dir.c_str());
	if(dp == 0)return;

	struct dirent *dirp;
	while((dirp = readdir(dp)) != 0)
	{
		string s = dir;
		s += "/";
		s += dirp->d_name;

		struct stat st;
		if(stat(s.c_str(), &st) == -1)continue;
		if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)continue;

		one_file of;
		of.name = s;
		of.size = st.st_size;
		of.last_write = st.st_mtime;
		of.dir = S_ISDIR(st.st_mode);
		v.push(of);

		total_size += of.size;

//		printf("%s,%d,%d,%d\n", s.c_str(), st.st_size, of.dir, of.last_write);

		if(S_ISDIR(st.st_mode)){
			list_dir(s, v, total_size);
		}
	}

	closedir(dp);
}

bool is_dir_empty(string& dir)
{
	DIR *dp = opendir(dir.c_str());
	if(dp == 0)return false;

	bool b = true;
	struct dirent *dirp;
	while((dirp = readdir(dp)) != 0)
	{
		string s = dir;
		s += "/";
		s += dirp->d_name;

		struct stat st;
		if(stat(s.c_str(), &st) == -1)continue;
		if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)continue;

		b = false;
		break;
	}

	closedir(dp);

	return b;
}

void try_clean(priority_queue<one_file>& v, unsigned& deled, unsigned will_del)
{
	while(!v.empty() && deled < will_del){
		one_file of = v.top();
		v.pop();
		if(!of.dir){
			printf("unlink \"%s\"\n", of.name.c_str());
			if(0 == unlink(of.name.c_str())){
				deled += of.size;
			}
			else{
			}
		}
		else if(is_dir_empty(of.name)){
			 rmdir(of.name.c_str());
		}
	}
}

double dir_freespace(const char * dir){
	struct statfs fs;
	fs.f_bavail = 0;
	if(statfs(dir, &fs) < 0)
	{
		printf("statfs fail 0x%x %s\n", errno, dir);
		return 0;
	}
	long long blocks,bfree,bavail;
	blocks = fs.f_blocks;
	bfree = fs.f_bfree;
	bavail = fs.f_bavail;

	double free = bavail * fs.f_bsize / Msize;
	return free;
}

double dir_totalspace(const char * dir){
	struct statfs fs;
	fs.f_bavail = 0;
	if(statfs(dir, &fs) < 0)
	{
		printf("statfs fail 0x%x %s\n", errno, dir);
		return 0;
	}
	long long blocks,bfree,bavail;
	blocks = fs.f_blocks;
	bfree = fs.f_bfree;
	bavail = fs.f_bavail;

	double total = blocks * fs.f_bsize / Msize;
	return total;
}

void create_dir(const char * dir){
	char buffer[1024];
	sprintf(buffer, "mkdir \"%s\" -p", dir);
	printf("%s\n", buffer);
	int ret = system(buffer);
	if(ret != 0){
		printf("create_dir fail %s\n", dir);
	}
}

#ifndef PATH_MAX 
#define PATH_MAX 1024 
#endif 

string get_exe_path(){ 
	char buf[PATH_MAX];
	int rslt = readlink("/proc/self/exe", buf, PATH_MAX);
	if(rslt < 0 || rslt >= PATH_MAX){
		return "";
	}
	buf[rslt] = '\0';
	char * zero = strrchr(buf, '/');
	*(++zero) = 0;
	return buf;
}

string g_cur_dir;

std::string current_dir(){
	if(g_cur_dir.empty()){
		g_cur_dir = get_exe_path();
		if(g_cur_dir.empty()){
			printf("get_exe_path err\n");
			char buf[512];
			return getcwd(buf, sizeof(buf));
		}
	}
	return g_cur_dir;
}

#endif

bool make_sure_path_exist(const char * path)
{
	if(0 == _access(path, 0))return true;
	create_dir(path);
	if(0 == _access(path, 0))return true;
	printf("make_sure_path_exist fail:%s\n", path);
	return false;
}

bool file_exist(const string& name){
	int ret = _access(name.c_str(), 0);
	return ret == 0;
}

bool get_ext_and_type(const string& fn, unsigned& type, string& ext)
{
	int index_dot_l = fn.find_last_of(".");
	if(index_dot_l == -1)return false;
	int index_dot_f = fn.find_last_of("-", index_dot_l - 1);
	if(index_dot_f == -1)return false;
	if(index_dot_f == index_dot_l)return false;

	ext.clear();
	ext.append(&fn[index_dot_l + 1]);
	string st;
	st.append(&fn[index_dot_f + 1], index_dot_l - index_dot_f - 1);

	sscanf(st.c_str(), "%x", &type);

	return true;
}
