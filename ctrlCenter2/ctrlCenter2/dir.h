#ifndef _DIR_H_
#define _DIR_H_

#ifdef WIN32
#include <direct.h>
#endif

#include <queue>
#include <string>
using namespace std;

struct one_file
{
	string        name;
	unsigned long size;
	unsigned long last_write;
	unsigned long dir;
};

bool operator < (const one_file & a, const one_file & b);

void create_dir(const char * dir);
void list_dir(const string& dir, priority_queue<one_file>& v, unsigned long long & total_size);
bool is_dir_empty(string& dir);
void try_clean(priority_queue<one_file>& v, unsigned& deled, unsigned will_del);
double dir_freespace(const char * dir); // 单位为M
double dir_totalspace(const char * dir); // 单位为M
bool make_sure_path_exist(const char * path);
std::string current_dir();
bool file_exist(const string& name);
bool get_ext_and_type(const string& fn, unsigned& type, string& ext);
#endif 
