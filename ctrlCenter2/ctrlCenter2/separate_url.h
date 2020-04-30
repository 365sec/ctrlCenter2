#ifndef _SEPARATE_H_
#define _SEPARATE_H_
#include <sstream>
#include <boost/regex.hpp>
#ifndef BOOST_NO_INTRINSIC_WCHAR_T
#define BOOST_NO_INTRINSIC_WCHAR_T
#include <boost/crc.hpp>
#undef BOOST_NO_INTRINSIC_WCHAR_T
#else
#include <boost/crc.hpp>
#endif
#include "base64.h"

bool separate_url(const string& url, string& host, string& port, string& file){
	std::string regstr = "http://([^/:]+):?(\\d*)(.*)";
	boost::regex expression(regstr);
	boost::smatch m;
	if(boost::regex_match(url, m, expression))
	{
		host.assign(m[1].first, m[1].second);
		port.assign(m[2].first, m[2].second);
		file.assign(m[3].first, m[3].second);

		if(port.empty()){
			port = "80";
		}
		if(file.empty()){
			file = "/";
		}
		return true;
	}
	return false;
}

bool crc_16_file(const string& filename, string& result){
	FILE * f = fopen(filename.c_str(), "rb");
	if(f){
		char buffer[1024];
		boost::crc_16_type crc16;
		int ret = 0;
		do{
			ret = fread(buffer, 1, sizeof(buffer), f);
			if(ret > 0){
				crc16.process_bytes(buffer, ret);
			}
		}
		while(ret == sizeof(buffer));
		fclose(f);
		sprintf(buffer, "%d", crc16.checksum());
		result = buffer;
		return true;
	}
	return false;
}


bool request_line(const string& str, string& method, string& page, vector< pair<string, string> >& args){
	
	std::string request_uri;
	std::string uri_regex = "([^ ]+) ([^ ]+) HTTP/1\\.[0|1]\r\n.+\r\n\r\n.*";
	boost::regex uri_expression(uri_regex);
	boost::smatch uri_m;
	if(boost::regex_match(str, uri_m, uri_expression))
	{
		if(uri_m[1].matched){
			method.assign(uri_m[1].first, uri_m[1].second);
		}
		if(uri_m[2].matched){
			request_uri.assign(uri_m[2].first, uri_m[2].second);
		}
	}

	if(request_uri.empty())return false;

	std::string page_regex("([^?]+).*");
	boost::regex page_expression(page_regex);
	boost::smatch page_m;
	if(boost::regex_match(request_uri, page_m, page_expression))
	{
		if(page_m[1].matched){
			page.assign(page_m[1].first, page_m[1].second);
		}
	}

	if(page.empty())return false;

	boost::regex reg("[?|&]([^=?&]+)=([^=?&]+)");
	boost::sregex_iterator it(request_uri.begin(), request_uri.end(), reg);
	boost::sregex_iterator end;
	while (it != end){
		string l, r;
		l.assign((*it)[1].first, (*it)[1].second);
		r.assign((*it)[2].first, (*it)[2].second);
		args.push_back(make_pair(l, r));
		it++;
	}
	return true;
}

bool header_field(const string& header, const string& field, string& s){
	string es = ".+?\r\n";
	es += field;
	es += ": ?(.+?)\r\n.+";
	boost::regex expression(es.c_str());
	boost::smatch m;
	if(boost::regex_match(header, m, expression))
	{
		if(m[1].matched){
			s.assign(m[1].first, m[1].second);
			return true;
		}
	}
	return false;
}

bool authorization(const string& header, string& username, string& password){
	string s;
	if(header_field(header, "Authorization", s)){
		string authorization = s.c_str() + 6;// skip "Basic "
		unsigned char out[32*1024];
		int out_length = sizeof(out);
		if(authorization.size() < (unsigned int)out_length/2){
			if(Base64Decode(authorization.c_str(), authorization.size(), out, &out_length)){
				out[out_length] = 0;
				boost::regex s_expression("([^:]*):(.*)");
				boost::smatch s_m;
				string s = (char*)out;
				if(boost::regex_match(s, s_m, s_expression))
				{
					if(s_m[1].matched){
						username.assign(s_m[1].first, s_m[1].second);
					}
					if(s_m[2].matched){
						password.assign(s_m[2].first, s_m[2].second);
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool content_length(const string& header, size_t& length){
	length = 0;
	string s;
	if(header_field(header, "Content-Length", s)){
		std::stringstream ss(s);
		ss >> length;
		return true;
	}
	return false;
}

bool content_type(const string& header, string& type, string& boundary){
	if(header_field(header, "Content-Type", type)){
		size_t i = type.find("; boundary=");
		if(i != string::npos){
			boundary = type.c_str() + i + 11;
			type.resize(i);
		}
		return true;
	}
	return false;
}

bool strip_header(string& s){
	size_t i = s.find("\r\n\r\n");
	if(i != string::npos){
		s = s.c_str() + i + 4;
		return true;
	}
	return false;
}

#endif // _SEPARATE_H_
