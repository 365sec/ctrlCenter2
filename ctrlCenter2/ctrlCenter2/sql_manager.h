#ifndef SQL_MANAGER_H
#define SQL_MANAGER_H

#include "both.h"
#include <iostream>
#include "SQLAPI.h"
#include "samisc.h"

//#include "nn.h"
//#include "pair.h"

#include "sync_map.hpp"

#include "log_utility.h"
#include "global.h"

using namespace std;

class sql_manager	
{
private:
	SAConnection sql_con_;
public:
	SAConnection *getSAConnection(){
		return &sql_con_;
	}
	bool connect_db(){
		try
		{
			sql_con_.Connect(
				g_db_name.c_str(),  // database name
				g_db_user.c_str(),				// user name
				g_db_passwd.c_str(),			// password
				SA_MySQL_Client);
			return true;
		}
		catch (SAException &x)
		{
			log_error(x.ErrText().GetMultiByteChars());
			close_db();
			return false;
		}
	}
	void close_db(){
		try
		{
			sql_con_.Disconnect();
		}
		catch (SAException &x)
		{
			log_error(x.ErrText().GetMultiByteChars());
		}
	}

	int handle_database_exception(SAException &x){
		std::string err = x.ErrText().GetMultiByteChars();
		if(err.find("MySQL server has gone away") != std::string::npos){
			//数据库服务器连接不上 网络原因
			close_db();
			connect_db();
		}
		else if(err.find("API client not set") != std::string::npos){
			close_db();
			connect_db();
		}
		else if(err.find("doesn't exist") != std::string::npos){
			//表或者数据库不存在不需要重连
		}
		return 0;
	}

	bool test_show_tables()
	{
		try{
			SACommand cmd(&sql_con_,"SHOW TABLES");
			cmd.Execute();
			while( cmd.FetchNext() )
			{
				for( int i = 1; i <= cmd.FieldCount(); ++i)
				{
					printf("%s: %s, %s\n",
						(const char*)cmd[i].Name(),
						(const char*)cmd[i].asBytes(),
						(const char*)cmd[i].asString());
				}
				printf("\n");
			}
			printf("\n");
			return true;
		}catch (SAException &x)
		{
			sa_printf(_TSA("ERR: "));
			sa_printf((const SAChar*)x.ErrText());
			sa_printf(_TSA("\n"));
			return false;
		}
	}
	bool test_select(){
		try{
			SACommand cmd(&sql_con_,"SELECT Id,PathName,StartTime from T_Flow_File WHERE ArchiveFlag = 0  ORDER BY Id LIMIT 100;");
			cmd.Execute();

			int count=0;
			long id=0;

			while (cmd.FetchNext())
			{
				count++;
				id=cmd[1].asLong();
				string filename(cmd[2].asString().GetMultiByteChars());
				SADateTime sadt=cmd[3].asDateTime();					
			}

			if(count==0)
			{
				cout<<"table is empty."<<endl;
			}
			return true;
		}catch (SAException &x)
		{
			sa_printf(_TSA("ERR: "));
			sa_printf((const SAChar*)x.ErrText());
			sa_printf(_TSA("\n"));
			return false;
		}
	}
	bool test_delete(){
		try{
			SACommand cmd;
			string tempcmd="DELETE FROM T_Flow_File ";
			tempcmd+="WHERE Id< 1000 and ArchiveFlag = 0;";
			cmd.setCommandText(tempcmd.c_str());
			cmd.Execute();
			return true;
		}catch (SAException &x)
		{
			sa_printf(_TSA("ERR: "));
			sa_printf((const SAChar*)x.ErrText());
			sa_printf(_TSA("\n"));
			return false;
		}
	}
};



#endif
