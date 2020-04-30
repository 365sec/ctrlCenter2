#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include "separate_url.h"
#include "dir.h"
#include <string>
#include <vector>
#include <sstream>

#include "boost/bind.hpp"
#include "boost/thread/recursive_mutex.hpp"

using namespace std;

typedef vector< pair<string, string> > args_t;
typedef pair<string, string> rets_t;
typedef boost::function<bool (const string&, args_t, rets_t&, const string&, const string&)> page_function_t;

bool get_arg(args_t args, const string& l, string& r){
	for(size_t i = 0; i < args.size(); ++i){
		if(l == args[i].first){
			r = args[i].second;
			return true;
		}
	}
	return false;
}

bool accounts_check(const string& u, const string& p);
//std::string current_dir();

class http_client{
	unsigned long long id_;
	string cache_;
	string method_;
	string page_;
	args_t args_;
	size_t content_length_;
	string content_type_;
	string boundary_;
	page_function_t deal_;
	string username_;
	string password_;
	bool request_line_ok_;
	asio_service::asio_service * server_;
public:
	http_client(unsigned long long id, const page_function_t& deal)
		: id_(id)
		, content_length_(0)
		, deal_(deal)
		, request_line_ok_(false)
		, server_(0)
	{
	}
	void set_server(asio_service::asio_service * s){
		server_ = s;
	}
	void session_ok(){
		cache_.clear();
		args_.clear();
		request_line_ok_ = false;
	}
	void on_recv(unsigned long long id, axis::shared_buffer& sb){
		cache_.assign((char *)sb.raw_data(), (char *)sb.raw_data() + sb.size());
		if(!request_line_ok_
			&& request_line(cache_, method_, page_, args_))
		{
			request_line_ok_ = true;
			authorization(cache_, username_, password_);
			if(check(username_, password_)){
				if(deal_){
					rets_t ret;
					bool b = deal_(page_, args_, ret, username_, password_);
					if(b){
						response(ret);
						session_ok();
					}
					else if(method_ == "POST"){
						content_length(cache_, content_length_);
						content_type(cache_, content_type_, boundary_);
						if(content_type_ == "multipart/form-data" && !boundary_.empty()){
							strip_header(cache_);
							multipart_form_data();
						}else{
							send_file(page_);
							session_ok();
						}
					}
					else{
						send_file(page_);
						session_ok();
					}
				}
			}
			else{
				need_authentication();
			}
		}
		if(request_line_ok_){
			if(content_type_ == "multipart/form-data" && !boundary_.empty()){
				multipart_form_data();
			}
		}
	}
	void multipart_form_data(){
	}
	void need_authentication(){
		const char* format =
			"HTTP/1.0 401 UNAUTHORIZED\r\n"
			"Server: box\r\n"
			"WWW-Authenticate: Basic realm=\"box\"\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 0\r\n"
			"\r\n";
		server_->tcp_send(id_, format, strlen(format));
	}
	bool check(const string& username, const string& password){
		//此处仅做了非空检测
		if(username.empty())return false;
		return true;
	}

	void send_file(const string& page){
		rets_t ret;
		string fn = current_dir();
		fn += page_;
		FILE * f = fopen(fn.c_str(), "rb");
		if(f){
			printf("send file %s\n", fn.c_str());
			char buffer[1024];
			while(true){
				int r = fread(buffer, 1, sizeof(buffer), f);
				if(r > 0 && r <= sizeof(buffer)){
					ret.second.append(buffer, r);
				}
				if(r != sizeof(buffer))break;
			}
			ret.first = "*/*";
			if(fn.find(".htm") != string::npos){
				ret.first = "text/html";
			}
			response(ret);
			fclose(f);
		}
		else{
			printf("not find file %s\n", fn.c_str());
			response_not_find();
		}
	}

	void response_not_find(){
		const char* format =
			"HTTP/1.1 404 Not Found\r\n"
			"Accept-Ranges: bytes\r\n"
			"Content-Length: 0\r\n" // ��λ�ֽ�
			"Content-Type: */*\r\n"
			"\r\n";
		server_->tcp_send(id_, format, strlen(format));
	}
	void response(const rets_t& ret){
		const char* format =
			"HTTP/1.1 200 OK\r\n"
			"Accept-Ranges: bytes\r\n"
			"Content-Length: %d\r\n" // ��λ�ֽ�
			"Content-Type: %s\r\n"
			"\r\n";
		char buffer[2048];
		sprintf(buffer, format, ret.second.size(), ret.first.c_str());
//		printf(buffer);
		server_->tcp_send(id_, buffer, strlen(buffer), ret.second.c_str(), ret.second.size());
	}
};

class http_service{
	typedef std::map<unsigned long long, http_client> client_container_t;
	typedef std::map<std::string, page_function_t> page_container_t;
	client_container_t clients_;
	page_container_t pages_;
	boost::recursive_mutex http_serv_mutex_;
public:
	http_service(){
		add_page("/add", boost::bind(&http_service::add, this, _1, _2, _3, _4, _5));
	}
	~http_service(){
	}
	void add_page(const string& page, page_function_t fun){
		boost::recursive_mutex::scoped_lock lock(http_serv_mutex_);//multithread need
		pages_.insert(make_pair(page, fun));
	}
	bool on_page(const string& page, args_t args, rets_t& ret, const string& user, const string& pass){
		boost::recursive_mutex::scoped_lock lock(http_serv_mutex_);//multithread need
		page_container_t::iterator iter = pages_.find(page);
		if(iter != pages_.end()){
			return iter->second(page, args, ret, user, pass);
		}
		return false;
	}
	bool add(const string& page, args_t args, rets_t& ret, const string&, const string&){
		int result = 0;
		for(size_t i = 0; i < args.size(); ++i){
			result += atoi(args[i].second.c_str());
		}
		stringstream ss;
		ss << "<?xml version=\"1.0\"?>\r\n";
		ss << "<?xml-stylesheet type=\"text/xsl\" href=\"/web/add.xsl\"?>\r\n";
		ss << "<result>" << result << "</result>\r\n";
		ret.second = ss.str();
		ret.first = "text/xml";
		return true;
	}
	void on_accept(unsigned long long id, asio_service::asio_service * s){
		http_client hc(id, boost::bind(&http_service::on_page, this, _1, _2, _3, _4, _5));
		hc.set_server(s);
		boost::recursive_mutex::scoped_lock lock(http_serv_mutex_);//multithread need
		clients_.insert(make_pair(id, hc));
	}
	bool on_recv(unsigned long long id, axis::shared_buffer& sb){
		boost::recursive_mutex::scoped_lock lock(http_serv_mutex_);//multithread need
		client_container_t::iterator iter = clients_.find(id);
		if(iter == clients_.end())return false;
		iter->second.on_recv(id, sb);
		return true;
	}
	void on_close(unsigned long long id){
		boost::recursive_mutex::scoped_lock lock(http_serv_mutex_);//multithread need
		clients_.erase(id);
	}
};

#endif
