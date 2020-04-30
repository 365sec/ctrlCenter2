#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using boost::bad_lexical_cast;
using std::vector;
using std::string;

class line_read
{
	string file_;
public:
	line_read(const string& file):file_(file){
	}
	~line_read(){
	}
	bool read_all(vector<string>& lines){
		FILE * f = fopen(file_.c_str(), "rb");
		if(f == 0)return false;

		fseek(f, 0, SEEK_END);
		unsigned file_len = ftell(f);
		fseek(f, 0, 0);
		if(0 == file_len){
			fclose(f);
			return false;
		}
		char *buf = new char[file_len];
		fread(buf, 1, file_len, f);
		unsigned last = 0;
		unsigned i = 0;
		for( i = 0; i < file_len; i++){
			if('\n' == buf[i]){
				string ts;
				if('\r' == buf[i-1]){
					ts.append(buf + last,i - last - 1);
				}else{
					ts.append(buf + last,i - last);
				}
				lines.push_back(ts);
				last = i + 1;
			}
		}
		if( last < file_len ){
			string ts;
			if('\r' == buf[i-1]){
				ts.append(buf + last,file_len - last - 1);
			}else{
				ts.append(buf + last,file_len - last);
			}
			lines.push_back(ts);
		}
		delete [] buf;
		fclose(f);
		return true;
	}
};

class read_ini{
public:
	read_ini(const string& filename){
		line_read file(filename);
		file.read_all(lines);
	}
	template <typename ValueType>
	bool find_value(const string key, ValueType& value){
		return find_value(lines,key,value);
	}
private:
	vector<string> lines;
	template <typename ValueType>
	bool find_value(const vector<string>& lines, const string key, ValueType& value){
		for( unsigned i = 0; i < lines.size(); i++){
			if(lines[i][0] == '!')continue;
			if(lines[i][0] == ';')continue;
			if(lines[i][0] == '#')continue;

			unsigned index = lines[i].find(key);
			if(index != -1){
				string ts = lines[i].substr(index + key.size());
				if(ts.length() != 0){
					try{
						value = lexical_cast<ValueType>(ts);
					}catch(bad_lexical_cast){
					}
				}
				return true;
			}
		}
		return false;
	}
};
