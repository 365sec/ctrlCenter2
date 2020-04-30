#ifndef MYNETSERVICE_H
#define MYNETSERVICE_H

#include <time.h>
#include "typedef.h"
#include <time.h>
#include <sstream>
#include <iostream>

#include "direct_data.h"
#include "configure.h"
#include "asio_service.hpp"
#include "http_service.h"
#include "lib_function.h"
#include "boost/algorithm/string.hpp"
#include "boost/unordered_map.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "log_utility.h"
#include "sync_map.hpp"
#include "NetWorld.h"
#include "ctrlCenter.h"

#define TICKPS 100	//每秒tick 100次

asio_service::asio_service *g_server;
extern std::string show_sessions();

using namespace std;
class tcp_user{
public:
	direct_data dd;
	unsigned waiting; 
	unsigned long long last_recv;//ticks
	boost::posix_time::ptime connect_time;
	boost::posix_time::ptime last_recv_time;
	string address;
	unsigned short port;
	int sessionid;

	typedef boost::function<void (unsigned long long ticks)> dealer_t;
	dealer_t deal_;
};

extern std::string show_session();


class myNetServ{
private:
	asio_service::asio_service * server_;
	http_service http_service_;

	typedef axis::sync_map<asio_service::id_t, tcp_user> tcp_user_container_t;
	typedef tcp_user_container_t::iter_t tcp_user_iter_t;
	tcp_user_container_t tcp_users_;

	unsigned long long timer_control_;

	bool m_login_server_connecting;
	unsigned long long m_login_server_id;

	boost::recursive_mutex net_serv_mutex_;



public:
	myNetServ()
		: server_(asio_service::create())
		, timer_control_(0)
		, m_login_server_connecting(false)
		, m_login_server_id(0)
	{
		g_server = server_;
		
		http_service_.add_page("/", boost::bind(&myNetServ::get_debug, this, _1, _2, _3, _4, _5));
		http_service_.add_page("/cmd", boost::bind(&myNetServ::get_cmd, this, _1, _2, _3, _4, _5));
		http_service_.add_page("/tasks", boost::bind(&myNetServ::get_tasks, this, _1, _2, _3, _4, _5));
		http_service_.add_page("/session", boost::bind(&myNetServ::get_session, this, _1, _2, _3, _4, _5));
	}

	~myNetServ(){
		cout<<"destroy net..."<<endl;
		server_->destroy();
	}
	

	int get_session_id(unsigned long long tcp_net_id)
	{
			tcp_user_iter_t  iter = tcp_users_.get(tcp_net_id);
			if(iter!=tcp_users_.end())
			{
				return iter->second.sessionid;
			}
			return -1;
	}

	void set_session_id(unsigned long long tcp_net_id, int sessionid )
	{
	    	tcp_user_iter_t  iter = tcp_users_.get(tcp_net_id);
			if(iter!=tcp_users_.end())
			{
				 iter->second.sessionid = sessionid;
			}
	}

	void close_tcp_id(unsigned long long tcp_net_id)
	{
		boost::recursive_mutex::scoped_lock lock(net_serv_mutex_);
	    server_->tcp_stop(tcp_net_id);
	}


	bool get_debug(const string& page, args_t args, rets_t& ret, const string& user, const string& pass){
		stringstream ss;
		ss << "----------base----------<br>";
		ss << "TICKPS:" << TICKPS << "<br>";
		ss << "version:" << g_version << "<br>";
		ss << "timer_control_:" << timer_control_ << "<br>";
		ss << "<br>";

		ret.second = ss.str();
		ret.first = "text/html";

		return true;
	}
	bool get_tasks(const string& page, args_t args, rets_t& ret, const string& user, const string& pass){
		stringstream ss;		
		stringstream session_ret;		
		session_ret<<"type\t\t id\t\t ssnid \t\t\tpid\t\tstatus"<<endl;
		std::map<int, DbTaskList *>::iterator itr;
		DbTaskList *pList = NULL;
		for(itr = g_mt.listCmd_.m_mapUsed.begin();itr != g_mt.listCmd_.m_mapUsed.end();itr++)
		{
			pList = itr->second;
			session_ret<<pList->nType<<"\t\t"
				<<pList->nID<<"\t\t"
				<<pList->nSsnID<<"\t\t"
				<<pList->nPID<<"\t\t"
				<<pList->nStatus<<endl;
		}
		ss <<"<strong>#session</strong><br>";
		ss << "<pre>";
		ss << session_ret.str();
		ss <<"</pre>";

		ret.second = ss.str();
		ret.first = "text/html";

		return true;
	}

	bool get_session(const string& page, args_t args, rets_t& ret, const string& user, const string& pass)
	{
       
	   ret.second = show_sessions();
	   ret.first = "text/html";
	   return true;
	}

	bool get_cmd(const string& page, args_t args, rets_t& ret, const string& user, const string& pass){
		stringstream ss;		
		stringstream session_ret;		
		session_ret<<"net_id\t\tip address\t\tport\t\tfirst connect\t\t\tlast recv time"<<endl;		
		tcp_user_iter_t iter = tcp_users_.begin();
		while(iter != tcp_users_.end()){

			session_ret<<iter->first<<"\t\t"
				<<iter->second.address<<"\t\t"
				<<iter->second.port<<"\t\t"
				<<boost::posix_time::to_simple_string(iter->second.connect_time)<<"\t\t"
				<<boost::posix_time::to_simple_string(iter->second.last_recv_time)<<endl;
			++iter;
		}
		ss <<"<strong>#session</strong><br>";
		ss << "<pre>";
		ss << session_ret.str();
		ss <<"</pre>";

		ret.second = ss.str();
		ret.first = "text/html";

		return true;
	}
	void start(){
		log_trace("ASIO service start");
		asio_service::function_handle_t timerh = boost::bind(&myNetServ::on_timer, this);
		server_->timer_set_handle(timerh);
		server_->timer_set_interval(1000/TICKPS); //10ms tick一次

		asio_service::tcp_handle th;
		th.accept_handle_ = boost::bind(&myNetServ::on_tcp_accept, this, _1, _2);
		th.connect_handle_ = boost::bind(&myNetServ::on_tcp_connect, this, _1, _2, _3, _4);
		th.close_handle_ = boost::bind(&myNetServ::on_tcp_close, this, _1);
		th.write_handle_ = boost::bind(&myNetServ::on_tcp_send, this, _1, _2);
		th.read_handle_ = boost::bind(&myNetServ::on_tcp_recv, this, _1, _2);

		server_->tcp_set_handle(th);


		if(!server_->tcp_accept_sync(tcp_http_string, "0.0.0.0", g_http_port, asio_service::no_split, true)){
			printf("tcp http bind failed on %d\n",g_http_port);
			exit(-1);
		}
		if(!server_->tcp_accept_sync(tcp_data_string, "0.0.0.0", g_listen_port, asio_service::no_split, true)){
			printf("tcp data bind failed\n");
			exit(-1);
		}                

		printf("server restart\n");
		server_->start_inplace(g_num_threads);
	}
	void stop()
	{
		server_->stop();
	}
	void on_tcp_send(asio_service::id_t id, axis::shared_buffer& sb){
		boost::recursive_mutex::scoped_lock lock(net_serv_mutex_);//multithread need
		tcp_user user;
		tcp_user_iter_t iter = tcp_users_.get(id);
		if(iter!=tcp_users_.end()){
			iter->second.waiting--;
		}

	}
	void add_user(asio_service::id_t id){
		if(id == 0){
			cout<<"add user 0!!!"<<endl;
			return;
		}
		tcp_user u;		
		u.waiting = 0;
		u.sessionid = -1;
		u.last_recv = timer_control_;
		u.connect_time = boost::posix_time::second_clock::local_time();
		u.dd.deal_ = boost::bind(&myNetServ::on_user_signal, this, id, _1, _2);

		server_->tcp_id_to_address(id,u.address,u.port);
		tcp_users_.insert(id, u);
	}
	void del_user(asio_service::id_t id){
		tcp_user u;
		tcp_users_.find_erase(id,u);
	}
	void on_tcp_accept(asio_service::id_t id, const std::string & name)
	{

		log_info("[on_tcp_accept]   has a new tcp accept");
		if(name == tcp_data_string){
			add_user(id);
		}
		else if(name == tcp_http_string){
			http_service_.on_accept(id, g_server);
		}
	}
	void on_tcp_close(asio_service::id_t id)
	{
		http_service_.on_close(id);
		printf("[server]connect  closed %d\n",id);

		log_info("[on_tcp_close]   closed %d\n",id);
		g_tWorld.OnSocketClosed(id);

		del_user(id);
		if(id == m_login_server_id){
			m_login_server_id = 0;
		}
	}

	void on_tcp_connect(const std::string& host_name, const std::string& service_name, asio_service::id_t id, void * private_data)
	{
		int pd = (intptr_t)private_data;
		log_info("[on_tcp_connect]   has a new tcp connect");
		if(pd == 'ls'){
			m_login_server_id = id;
			m_login_server_connecting = false;

			if(id != 0){
				add_user(m_login_server_id);
			}
		}
	}

	void on_tcp_recv(asio_service::id_t id, axis::shared_buffer& sb)
	{

		if(http_service_.on_recv(id, sb)){			
			return;
		}
		else{
			tcp_user u;
			tcp_user_iter_t  iter = tcp_users_.get(id);
			if(iter!=tcp_users_.end()){
				iter->second.last_recv = timer_control_;
				iter->second.last_recv_time = boost::posix_time::second_clock::local_time();
				bool ok = iter->second.dd.data_in((byte*)sb.raw_data(), sb.size());//data_in的同时，进行粘包处理，如果有完整包，则回调处理。
				if(!ok){
					cout<<"[on_tcp_recv]handle error, stop link:"<<id<<endl;
					log_error("[on_tcp_recv]tcp_id(%d),handle error, to be closed !",id);
					server_->tcp_stop(id);
				}
			}
		}
	}

	bool on_user_signal(unsigned long long tcp_net_id, byte * dat, size_t size1)
	{	
		//业务逻辑处理，返回FLASE 表示处理出错，客户端将被关闭
		return g_tWorld.HandlePacket(tcp_net_id,dat,size1);
	}

	
	void check_deadlink(){ 
		
		boost::recursive_mutex::scoped_lock lock(net_serv_mutex_);//multithread need

		for (tcp_user_iter_t iter = tcp_users_.begin();iter!= tcp_users_.end();)
		{
			if(timer_control_-iter->second.last_recv > 600 * TICKPS){//超过100秒，则断开连接，删除节点。
				tcp_user_iter_t iter_temp = iter;
				iter++;
				log_info("tcp_id(%d) time out need to be closed\n",iter_temp->first);
				server_->tcp_stop(iter_temp->first);
			}else{
				iter++;
			}
		}
	}

	void on_timer()
	{            
		timer_control_++;
		if(timer_control_ % (100*TICKPS) == 100){ // 100秒check 一次     
			check_deadlink();
		}
	}

	void send_to_user(unsigned long long tcp_net_id, axis::shared_buffer& sb)
	{
		if(tcp_net_id){
			tcp_user_iter_t iter = tcp_users_.get(tcp_net_id);
			if(iter!=tcp_users_.end()){
				if(iter->second.waiting>25){
					stringstream ss;
					ss<<"user "<<tcp_net_id<<" waste time,drop it";
					log_warning(ss.str().c_str());
					return;
				}
				iter->second.waiting++;
				server_->tcp_send(tcp_net_id,sb);
			}
		}
	}
	void send_to_user(unsigned long long tcp_net_id, byte * data1, size_t size1, byte * data2 = 0, size_t size2 = 0, byte * data3 = 0, size_t size3 = 0)
	{
		if(tcp_net_id){
			tcp_user_iter_t iter = tcp_users_.get(tcp_net_id);
			if(iter!=tcp_users_.end()){
				iter->second.waiting++;
				server_->tcp_send(tcp_net_id, data1, size1, data2, size2, data3, size3);
			}
		}
	}

	void send_to_all_user(byte * data1, size_t size1, byte * data2 = 0, size_t size2 = 0, byte * data3 = 0, size_t size3 = 0)
	{
		tcp_user_iter_t iter = tcp_users_.begin();
		while(iter != tcp_users_.end()){
			iter->second.waiting++;
			server_->tcp_send(iter->first, data1, size1, data2, size2, data3, size3);
			++iter;
		}
	};
};




#endif